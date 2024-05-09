// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TargettingComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h" 
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Weapons/BasicPistol.h"
#include "Items/Weapons/BasicShield.h"
#include "Items/TwoHandedWeapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "HUD/SlashOverlay_2.h"


// Sets default values
ASlashCharacter::ASlashCharacter()
{
	// turning off rotation of controller for character's body
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// to ORIENT character towards movement in blueprint we can tick Character Movement component's detail panel Orient Rotation Movement. Rotation Rate determines how slowly character is rotating
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 320.f, 0.f);

	//Handling Collision Settings
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);


 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setting up SpringArm and its attachment and length
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	//CameraBoom->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

	//Setting up a Camera component and attaching it to the CameraBoom
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	//Constructing Hair from UGroomComponent and attaching it to main character mesh
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	//CHaracters have sockets which are special places we can attach objects. Head is one of them
	Hair->AttachmentName = FString("head");

	// Same thing for Eyebrows
	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	TargeterSystem = CreateDefaultSubobject<UTargettingComponent>(TEXT("Systems"));
	TargeterSystem->FollowCamera = ViewCamera;
	
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Adding Input Mapping Context
	if (APlayerController* PlayerController = Cast< APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// SlashContext need to be set in blueprint
			Subsystem->AddMappingContext(SlashContext, 0);
		}
		// GetHUD and cast it to SlashHUD
		InitializeSlashOverlay(PlayerController);
	}

	Tags.Add(FName("EngageableTarget"));

	SpawnDefaultWeapon(FName("RightHandSocket")); // Initialize Starting Weapon
	SpawnDefaultPistol(FName("PistolSocket"));
	CharacterState = ECharacterState::ECS_NoShield;
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_Dodge && ActionState != EActionState::EAS_Shooting &&
		ActionState!= EActionState::EAS_Blocking && ActionState!= EActionState::EAS_UsingConsumable) return;
	//Movement vector will get X and Y values for movementvector depending on what keys are we pressing
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	// Moving in the direction of the controller's (mouse's) rotation and not the character's rotation
	// We only care here about yaw (horizontal) rotation of the mouse controller.
	const FRotator Rotation = Controller->GetControlRotation(); // returns Controller rotation
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	// Right Vector is for moving to the right and left based on camera vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);


}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("LookAxisVector: %s"), *LookAxisVector.ToString());

	if (TargeterSystem->bIsTargetting && LookAxisVector.X < 1.5 && LookAxisVector.X > -1.5 && LookAxisVector.Y > -0.2 && LookAxisVector.Y < 0.2) return;
	// If look axis vector wiekszy to Targeter system zmieñ target. Na prawo > 0 a na lewo < 0
	if (TargeterSystem->bIsTargetting && LookAxisVector.X > 1.4 || LookAxisVector.X < -1.4)
	{
		TargeterSystem->ChangeTarget(LookAxisVector.X);
	}

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void ASlashCharacter::EKeyPressed()
{ 

	if (ABasicShield* OverlappingShield = Cast< ABasicShield>(OverlappingItem))
	{
		EquipShield(OverlappingShield);
	}

	else if (AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
	{
		EquipOneHanded(OverlappingWeapon);
	}
	else
	{

		const bool bCanDisarm = ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_EquippedShield && bHasShield;
		//const bool bCanArm = ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
		const bool bCanArm = ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_EquippedShield;
		if (bCanDisarm)
		{
			
			PlayEquipMontage(FName("Unequip"));

		}
		else if (bCanArm)
		{
			PlayEquipMontage(FName("Equip"));

			//CharacterState = ECharacterState::ESC_EquippedOneHandedWeapon;
			//ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::EquipOneHanded(AWeapon* OverlappingWeapon)
{
	OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);

	//Changing the Character State to ECS_EquippedOneHandedWeapon
	//CharacterState = ECharacterState::ECS_NoShield;
	OverlappingItem = nullptr;
	EquippedWeapon = OverlappingWeapon;
}

void ASlashCharacter::EquipShield(ABasicShield* OverlappingShield)
{
	OverlappingShield->Equip(GetMesh(), FName("LeftHandSocket"), this, this);

	//Changing the Character State to ECS_EquippedOneHandedWeapon
	CharacterState = ECharacterState::ECS_EquippedShield;
	//EquippedWeapon = OverlappingShield;
	OverlappingItem = nullptr;
	EquippedShield = OverlappingShield;
	bHasShield = true;
}

void ASlashCharacter::StartSprint()
{
	if (CharacterState == ECharacterState::ECS_NoShield)
	{
		GetCharacterMovement()->MaxWalkSpeed = NoShieldSprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WithShieldSprintSpeed;
	}
	bIsSprinting = true;
}

void ASlashCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 580;
	bIsSprinting = false;
}

void ASlashCharacter::Attack()
{
	Super::Attack();
	IsPlayerAttacking = true;
	CheckCharacterSpeed(); //UpdateCharacterSpeed
	const bool bCanAttack = (ActionState == EActionState::EAS_Unoccupied || ActionState == EActionState::EAS_Blocking || ActionState == EActionState::EAS_Parrying) && CharacterState != ECharacterState::ECS_Unequipped && HasStamina();
	if (bCanAttack)
	{
		//DrainStamina(Attributes->GetStaminaAttackCost());
		//PlayAttackMontage();
		//ActionState = EActionState::EAS_Attacking;
		if (CharacterSpeed >= 700.f)
		{
			PlayJumpAttackMontage();
			DrainStamina(Attributes->GetStaminaAttackCost());
			//ActionState = EActionState::EAS_Attacking;
		}
		else
		{
			// Here it will Check Enum AttackType. And Perform Normal or Heavy AttackCombo depending on the value. Attack function callback for heavy and normal attacks  will first set the AttackType and the perform general Heavy Attack Function.
			// Now need to set the
			if (AttackType == EAttackType::EAT_NormalAttack)
			{
				if (CharacterState == ECharacterState::ECS_EquippedShield) PlayShieldComboAttack();
				if (CharacterState == ECharacterState::ECS_NoShield) PlayNoShieldComboAttack();
				//PlayShieldComboAttack();
			}

			if (AttackType == EAttackType::EAT_HeavyAttack)
			{
				EquippedWeapon->MultiplyDamage(HeavyDamageMultiplicator);
				PlayHeavyComboAttack();
				DrainStamina(Attributes->GetStaminaAttackCost());
				UE_LOG(LogTemp, Warning, TEXT("HEAVY ATTACK PERFORMED!!!!!"));
			}

			//PlayAttackMontage();
			ActionState = EActionState::EAS_Attacking;
		}
	}
	else
	{
		IsAttackSaved = true;
	}
	

}

void ASlashCharacter::DefensiveAction()
{
	if (CharacterState == ECharacterState::ECS_NoShield)
	{
		Parry();
	}
	else
	{
		Block();
	}
}

void ASlashCharacter::Shoot()
{
	EquippedPistol->FirePistol(CombatTarget);
}

void ASlashCharacter::Block()
{
	Super::Block();
	if (bHasShield && ActionState != EActionState::EAS_HitReaction && ActionState != EActionState::EAS_Staggered)
	{
		ActionState = EActionState::EAS_Blocking;
	}
}

void ASlashCharacter::AimToShoot()
{
	bool bCanShoot = (ActionState == EActionState::EAS_Unoccupied) && CharacterState == ECharacterState::ECS_NoShield;
	if (bCanShoot)
	{
		ActionState = EActionState::EAS_Shooting;
		PlayShootMontage(FName("Shoot"));
	}
}

void ASlashCharacter::EndBlock()
{
	if (bHasShield && CharacterState == ECharacterState::ECS_EquippedShield)
	{
		UE_LOG(LogTemp, Warning, TEXT("End Block PLayed"));
		ActionState = EActionState::EAS_Unoccupied;
		bIsBlocking = false;
		IsBlockActive = false;
		IsParryActive = false;
		IsPlayerAttacking = false;
	}
}

void ASlashCharacter::Parry()
{
	if (ActionState != EActionState::EAS_HitReaction && ActionState != EActionState::EAS_Staggered)
	{
		ActionState = EActionState::EAS_Parrying;
		//IsBlockActive = true;
		IsParryActive = true;
		PlayParryMontage();
	}

}

void ASlashCharacter::NormalAttack()
{
	AttackType = EAttackType::EAT_NormalAttack;
	Attack();
}

void ASlashCharacter::HeavyAttack()
{
	AttackType = EAttackType::EAT_HeavyAttack;
	Attack();
}

void ASlashCharacter::Dodge()
{
	// Can't dodge if is in any other state than unoccupied or attacking and has to have stamina
	if (ActionState != EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_Attacking || !HasStamina()) return;
	if (CharacterState == ECharacterState::ECS_NoShield)
	{
		PlayDodgeMontage(FName("NoShield"));
	}
	else
	{
		PlayDodgeMontage(FName("Shield"));
	}
	
	ActionState = EActionState::EAS_Dodge;
	DrainStamina(Attributes->GetStaminaDodgeCost());
}

void ASlashCharacter::AcquireTarget()
{
	if (CharacterState != ECharacterState::ECS_Unequipped)
	{
		if (!TargeterSystem->bIsTargetting)
		{
			TargeterSystem->EnableLockOn();
			SetCombatTarget(TargeterSystem->TargetActor);
		}
		else if (TargeterSystem->bIsTargetting)
		{
			TargeterSystem->DisableLockOn();
			CombatTarget = nullptr;
		}
	}
}

void ASlashCharacter::UseCurrentConsumable()
{
	if (ActionState == EActionState::EAS_Unoccupied && NumHealingPotions > 0)
	{
		PlayConsumableMontage();
		ActionState = (EActionState::EAS_UsingConsumable);

	}

}

void ASlashCharacter::ParryEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	//bIsBlocking = false;
	IsPlayerAttacking = false;
	IsParryActive = false;
}

void ASlashCharacter::ActivateConsumablesEffect()
{
	if (NumHealingPotions > 0)
	{
		NumHealingPotions -= 1;
		Attributes->Heal(HealthPotionStrength);
		SetHUDHealth();
		SlashOverlay->SetConsumable(NumHealingPotions);
	}
}

//int32 ASlashCharacter::PlayJumpAttackMontage()
//{
	//return PlayRandomMontageSection(JumpAttackMontage, JumpAttackMontageSections);
//}

void ASlashCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::ContinueAttack()
{
	if (IsAttackSaved)
	{
		if (AttackType == EAttackType::EAT_NormalAttack)
		{
			if (CharacterState == ECharacterState::ECS_EquippedShield) PlayShieldComboAttack();
			else if (CharacterState == ECharacterState::ECS_NoShield) PlayNoShieldComboAttack();
			
			ActionState = EActionState::EAS_Attacking;
		}
		if (AttackType == EAttackType::EAT_HeavyAttack)
		{
			PlayHeavyComboAttack();
			ActionState = EActionState::EAS_Attacking;
		}
	}
}

void ASlashCharacter::AttackEnd()
{
	Super::AttackEnd();
	ActionState = EActionState::EAS_Unoccupied;
	EquippedWeapon->SetBasicDamage();
	IsStaggered = false;
	IsPlayerAttacking = false;

	ComboCounter = 0; // When Attack End is Reached we need to reset the combo 
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
	IsPlayerAttacking = false;
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();
	ActionState = EActionState::EAS_Dead;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASlashCharacter::Disarm()
{
	if (EquippedShield)
	{
		EquippedShield->AttachMeshToSocket(GetMesh(), FName("ShieldSpineSocket"));
		bHasShield = false;
		CharacterState = ECharacterState::ECS_NoShield;
	}
}

void ASlashCharacter::Arm()
{
	/*
	if (Cast<ATwoHandedWeapon>(EquippedWeapon))
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("Two_Handed_Socket"));
	}

	else if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
	*/
	if (EquippedShield)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquippedShield"));
		EquippedShield->AttachMeshToSocket(GetMesh(), FName("LeftHandSocket"));
		bHasShield = true;
		CharacterState = ECharacterState::ECS_EquippedShield;
	}
}

void ASlashCharacter::PistolDisarm()
{
	if (EquippedPistol)
	{
		EquippedPistol->AttachMeshToSocket(GetMesh(), FName("PistolSocket"));
	}
}

void ASlashCharacter::PistolArm()
{
	if (EquippedPistol)
	{
		EquippedPistol->AttachMeshToSocket(GetMesh(), FName("PistolHandSocket")); 
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	IsStaggered = false;
	IsPlayerAttacking = false;
}

void ASlashCharacter::IntouchabilityOn()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
}

void ASlashCharacter::IntouchabilityOff()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
}

void ASlashCharacter::InitializeSlashOverlay(APlayerController* PlayerController)
{
	ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());

	if (SlashHUD)
	{
		SlashOverlay = SlashHUD->GetSlashOverlay();

		if (SlashOverlay && Attributes)
		{
			SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
			SlashOverlay->SetStaminaBarPercent(1.f);
			SlashOverlay->SetGold(0);
			SlashOverlay->SetSouls(0);
			SlashOverlay->SetConsumable(NumHealingPotions);
		}
	}
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("IsBlockActive: %s"), IsBlockActive ? TEXT("true") : TEXT("false"));
	Super::Tick(DeltaTime);
	if (Attributes && SlashOverlay)
	{
		if (ActionState != EActionState::EAS_Blocking)
		{
			Attributes->RegenStamina(DeltaTime);
		}
		if (bIsSprinting)
		{
			Attributes->SprintDrainStamina(DeltaTime);
		}
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());

		if (TargeterSystem == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("WE HAVE A PROBLEM!!!!"));
		}
	}
}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// binding input action variable MovementAction to function Move
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Started, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ASlashCharacter::NormalAttack);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ASlashCharacter::AimToShoot);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Triggered, this, &ASlashCharacter::DefensiveAction);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &ASlashCharacter::EndBlock);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASlashCharacter::Dodge);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASlashCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASlashCharacter::StopSprint);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Completed, this, &ASlashCharacter::HeavyAttack);
		EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Completed, this, &ASlashCharacter::AcquireTarget);
		EnhancedInputComponent->BindAction(ConsumableAction, ETriggerEvent::Completed, this, &ASlashCharacter::UseCurrentConsumable);
	}

}

void ASlashCharacter::Jump()
{
	if (ActionState == EActionState::EAS_Unoccupied && HasStamina())
	{
		DrainStamina(Attributes->GetStaminaDodgeCost());
		UE_LOG(LogTemp, Warning, TEXT("MyFloatVariable: %f"), Attributes->GetStaminaDodgeCost());
		Super::Jump();
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (ActionState != EActionState::EAS_HitReaction)
	{
		HandleDamage(DamageAmount);
		SetHUDHealth();
		return DamageAmount;
	}
	else return 0;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	// If Hit angle falls between -120 and 120 and EAS_Blocking we dont get actual Hit Implementation
	double HitAngle = GetAngleofHit(Hitter->GetActorLocation());
	//UE_LOG(LogTemp, Warning, TEXT("HitAngle is: %f"), HitAngle);
	IHitInterface* HitterInterface = Cast<IHitInterface>(Hitter);

	//if (ActionState == EActionState::EAS_HitReaction) return;
	//if (HitAngle >= -120.f && HitAngle < 120.f && ActionState == EActionState::EAS_Blocking  && ActionState != EActionState::EAS_Staggered && HasStamina())
	if (HitAngle >= -120.f && HitAngle < 120.f && (IsBlockActive || IsParryActive) && ActionState != EActionState::EAS_Staggered && HasStamina() && !HitterInterface->bAttackUnblockable)  // Perform Block
	{ 
		if (BlockReactMontage)
		{
			bIsBlocking = true;
			if (ActionState == EActionState::EAS_Blocking)
			{
				PlayMontageSection(BlockReactMontage, FName("Default"));
				DrainStamina(EquippedWeapon->GetDamage() * 2);
				if (IsBlockActive)
				{
					UE_LOG(LogTemp, Warning, TEXT("PLAY BLOCK SOUND!!!!"))
					PlayBlockSound(ImpactPoint);
				}
			}
			if (IsParryActive)
			{
				UE_LOG(LogTemp, Warning, TEXT("PLAY PARRY SOUND!!!!"))
				PlayParrySound(ImpactPoint);
			}
			Super::GetHit_Implementation(ImpactPoint, Hitter);
			bIsBlocking = false;
		}
	}
	else if (HitAngle >= -120.f && HitAngle < 120.f && ActionState == EActionState::EAS_Blocking && !HasStamina() && !HitterInterface->bAttackUnblockable) // Get Staggered
	{
		PlayMontageSection(StaggerMontage, FName("Default"));
		ActionState = EActionState::EAS_Staggered;
		IsStaggered = true;
		bIsBlocking = false;
	}
	else // Take a Hit
	{
		Super::GetHit_Implementation(ImpactPoint, Hitter);
		UE_LOG(LogTemp, Warning, TEXT("Hit wasnt blocked"));
		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

		if (Attributes && Attributes->GetHealthPercent() > 0.f)
		{
			ActionState = EActionState::EAS_HitReaction;
		}
	}
}

void ASlashCharacter::HitRebound_Implementation(AActor* Hitter)
{
	ActionState = EActionState::EAS_HitReaction;
	PlayHitReboundMontage();
}

void ASlashCharacter::DrainStamina(float Usage)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->SetStaminaCost(Usage);
		Attributes->UseStamina(Attributes->GetStaminaCost());
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

bool ASlashCharacter::HasStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetStaminaCost();
}

int32 ASlashCharacter::SelectComboSection(UAnimMontage* Montage, const TArray<FName>& SectionNames, int32& Counter)
{
	if (HasStamina())
	{
		Super::SelectComboSection(Montage, SectionNames, Counter);
		DrainStamina(Attributes->GetStaminaAttackCost());
		return Counter;
	}
	return 0;
}

int32 ASlashCharacter::PlayNoShieldComboAttack()
{
	return SelectComboSection(NoShieldAttackMontage, NoShieldAttackMontageSections, ComboCounter);
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}


void ASlashCharacter::CheckCharacterSpeed()
{
	FVector Velocity = GetCharacterMovement()->Velocity;
	FVector GroundVelocity = FVector(Velocity.X, Velocity.Y, 0.0f);
	CharacterSpeed = GroundVelocity.Size();
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ASlashCharacter::SetCombatTarget(AActor* Target)
{
	CombatTarget = Target;
}



