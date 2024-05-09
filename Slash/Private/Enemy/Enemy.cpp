// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"


// Constructor - Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setting collision channel to World Dynamic and to block visibility channel (Setting it up to be hit by weapon)
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// Ignoring enemie's mesh or capsule when it collides with camera
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Setup mesh to generate overlap events
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	// Orient character always towards its natural movement direction and not the target
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Instantiating PawnSensing Component and adjusting its features
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 3700.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	LockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOn"));
	LockOnWidget->SetupAttachment(GetRootComponent());
	DisableLockOnWidget();
	//LockOnWidget = CreateWidget<UUserWidget>(GetWorld(), LockOnWidgetClass);
	//Tags.Add(FName("Enemy"));

}

void AEnemy::EnableLockOnWidget()
{
	LockOnWidget->SetVisibility(true);
}

void AEnemy::DisableLockOnWidget()
{
	LockOnWidget->SetVisibility(false);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState == EEnemyState::EES_Dead) return;
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	// Setting up a Combat target as a Pawn of instigator of a damage and start chasing
	CombatTarget = EventInstigator->GetPawn();

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	//UE_LOG(LogTemp, Warning, TEXT("GOT HIT ACTIVATED"));
	if (EnemyState == EEnemyState::EES_Block)
	{
		PlayMontageSection(BlockReactMontage, FName("Default"));
		PlayParrySound(ImpactPoint);
	}
	else
	{
		EnemyState = EEnemyState::EES_HitReaction;
		Super::GetHit_Implementation(ImpactPoint, Hitter);
		CombatTarget = Hitter;
		if (EnemyState != EEnemyState::EES_Dead) ShowHealthBar();
		ClearPatrolTimer();
		ClearAttackTimer();
		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
		// Add a Notify at the end of Hit React that will activate DodgeRandomizer
	}
	
}

void AEnemy::HitRebound_Implementation(AActor* Hitter)
{
	EnemyState = EEnemyState::EES_HitReaction;
	PlayHitReboundMontage();
}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();
	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetActorTickEnabled(false);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnSoul();
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		const FVector SoulSpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 20.f);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SoulSpawnLocation, GetActorRotation());

		if (SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSouls());
		}
		else if (SpawnedSoul == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("NO SOUL SPAWNED"));
		}
	}
}

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr) return;

	float RandomNumber = FMath::FRandRange(0.0f, 1.0f);
	float RandomNumber2 = FMath::FRandRange(0.0f, 1.0f);
	float RandomNumber3 = FMath::FRandRange(0.0f, 1.0f);
	// Either Attack or Movement (dodge or circle around)
	// If CombatTarget is Staggered there is a higher chance of attacking
	if (Cast<ABaseCharacter>(CombatTarget)->IsStaggered)
	{
		if (RandomNumber3 < PercentOfStaggerPunishment)
		{
			AttackRandomizer(RandomNumber2);
		}
	}
	else
	{
		if (RandomNumber3 < PercentOfAttacks)
		{
			AttackRandomizer(RandomNumber2);
		}
		else if (RandomNumber3 < PercentOfAttacks + PercentOfBlocks)
		{
			Block();
			UE_LOG(LogTemp, Warning, TEXT("Standard Block is playing"));
		}
		else
		{
			PerformCombatMovement(RandomNumber);
		}
	}
	
/*
	if (RandomNumber3 <= PercentofCombatMovement)
	{
		PerformCombatMovement(RandomNumber);
	}
	else
	{
		AttackRandomizer(RandomNumber2);
	}
	*/
}

void AEnemy::PerformCombatMovement(float RandomNumber)
{
	if (RandomNumber <= PercentOfDodges)
	{
		Dodge();
	}
	else
	{
		CircleAround();
	}
}

void AEnemy::AttackRandomizer(float RandomNumber2)
{
	// Here the attack will be randomized to play either Heavy or Standard
	EnemyState = EEnemyState::EES_Engaged;
	if (RandomNumber2 <= PercentOfLightAttacks)
	{
		PlayAttackMontage();

	}
	else
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->MultiplyDamage(HeavyDamageMultiplicator);
			PlayHeavyAttackMontage();
		}
		
	}
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		EnemyState != EEnemyState::EES_Attacking &&
		EnemyState != EEnemyState::EES_Engaged &&
		EnemyState != EEnemyState::EES_Dodge &&
		EnemyState != EEnemyState::EES_HitReaction &&
		EnemyState != EEnemyState::EES_Block &&
		EnemyState != EEnemyState::EES_Dead;
	return bCanAttack;
}

bool AEnemy::CanJumpAttack()
{
	bool bCanAttack =
		IsInsideJumpAttackRadius() &&
		!IsInsideAttackRadius() &&
		EnemyState != EEnemyState::EES_Attacking &&
		EnemyState != EEnemyState::EES_Engaged &&
		//EnemyState != EEnemyState::EES_Dodge &&
		EnemyState != EEnemyState::EES_HitReaction &&
		EnemyState != EEnemyState::EES_Block &&
		EnemyState != EEnemyState::EES_Dead;
	return bCanAttack;
}

bool AEnemy::CanDodge()
{
	bool bCanDodge =
		EnemyState != EEnemyState::EES_Dodge &&
		EnemyState !=EEnemyState::EES_HitReaction &&
		EnemyState != EEnemyState::EES_Engaged &&
		EnemyState != EEnemyState::EES_Dead;
	return bCanDodge;
}

void AEnemy::AttackEnd()
{
	Super::AttackEnd();
	// Afetr performing an attack we set the state to empty NoState and the determining the state in CheckCombatTarget
	EnemyState = EEnemyState::EES_NoState;
	//EquippedWeapon->SetBasicDamage();
	CheckCombatTarget();
}

void AEnemy::Block()
{
	Super::Block();
	if (CanDodge())
	{
		EnemyState = EEnemyState::EES_Block;
		PlayBlockMontage();
	}
}

void AEnemy::Dodge()
{
	if (CanDodge())
	{
		EnemyState = EEnemyState::EES_Dodge;
		PlayDodgeMontage(FName("Default"));
	}
}

void AEnemy::CircleAround()
{
	if (CanDodge())
	{
		EnemyState = EEnemyState::EES_Dodge;
		PlayCircleAroundMontage();
	}
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::BeginBlock()
{
	EnemyState = EEnemyState::EES_Block;
	IsBlockActive = true;
}

void AEnemy::CombackBlockRandomizer()
{
	float RandomNumber = FMath::FRandRange(0.0f, 1.0f);
	if (RandomNumber <= PercentOfComebackBlock)
	{
		EnemyState = EEnemyState::EES_NoState;
		Block();
	}
}

void AEnemy::CombackAttackRandomizer()
{
	float RandomNumber = FMath::FRandRange(0.0f, 1.0f);
	float RandomNumber2 = FMath::FRandRange(0.0f, 1.0f);
	if (RandomNumber <= PercentOfComebackAttack)
	{
		EnemyState = EEnemyState::EES_NoState;
		AttackRandomizer(RandomNumber2);
	}
}

void AEnemy::InitializeEnemy()
{
	// Geting hold of enemy controller. Needs to be done before doing anything with enemies movement.
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon(FName("WeaponSocket")); // Spawning a weapon for the enemy
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	// True if DistanceToTarget is within the acceptable radius
	return DistanceToTarget <= Radius;
}


void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr || EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Block) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);

}

void AEnemy::CheckCombatTarget()
{
	//FString EnumString = UEnum::GetValueAsString(EnemyState); // Convert ENUM value to FString
	//UE_LOG(LogTemp, Warning, TEXT("Enum value is: %s"), *EnumString);
	if (EnemyState == EEnemyState::EES_Dead) return;
	if (IsOutsideCombatRadius())  //Outside combat radius lose interest
	{
		ClearAttackTimer();
		LoseInterest();
		if (EnemyState != EEnemyState::EES_Engaged)
		{
			StartPatrolling();
		}
	}

	else if (IsOutsideAttackRadius() && EnemyState != EEnemyState::EES_Chasing)
	{
		if (EnemyState == EEnemyState::EES_Block) return;
		ClearAttackTimer();
		if (EnemyState != EEnemyState::EES_Engaged && EnemyState != EEnemyState::EES_Block)
		{
			ChaseTarget();
		}
	}
	// If is inside JumpAttack Radius && SprintAttack is not a null pointer
	// Randomizer PerformSprintAttack or ChaseTarget
	else if (CanJumpAttack())
	{
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::JumpAttackRandomizer, 0.01f);
		//JumpAttackRandomizer();
	}
	else if (CanAttack())
	{
		StartAttackTimer();
		
	}
	
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float SecondsDelay = FMath::RandRange(5.f, 10.f);// Get random float for delay seconds
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, SecondsDelay);
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));
	//We could try to cast SeenPawn to ASlashCharacter type but PawnSeen is called to frequently to do Casting so often
	// That's why we use Actor's tag to check if SeenPawn has the specified tag

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer(); //We need to clear the timer not to trigger the next patrol target
		ChaseTarget();
	}
}

//Returns a random patrol that is not already being used
AActor* AEnemy::ChoosePatrolTarget()	
{	
	// add to Valid Targets all the points that are not the current target
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	//Choosing another patrol target after reaching the prior one by generating a random number to set random jump between patrol points
	if (PatrolTargets.Num() > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, ValidTargets.Num() - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}
/*
void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}
*/

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);

}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	if (EnemyState == EEnemyState::EES_Block) return;
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

void AEnemy::JumpAttackRandomizer()
{
	if (CombatTarget == nullptr) return;
	float RandomNumber = FMath::FRandRange(0.0f, 1.0f);
	float RandomNumber2 = FMath::FRandRange(0.0f, 1.0f);
	if (RandomNumber <= PercentOfJumpAttacks)
	{
		EnemyState = EEnemyState::EES_Engaged;
		PlayJumpAttackMontage();
	}
	else  //Circle Around or Approach the Target
	{
		if (RandomNumber2 <= PercentOfDodges)
		{
			CircleAround();
		}
		else
		{
			if (EnemyState == EEnemyState::EES_Block) return;
			GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
			MoveToTarget(CombatTarget);
		}
	}
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius); // Here we have the negation of the function (!)
}

bool AEnemy::IsInsideJumpAttackRadius()
{
	return InTargetRange(CombatTarget, JumpAttackRadius);
}





