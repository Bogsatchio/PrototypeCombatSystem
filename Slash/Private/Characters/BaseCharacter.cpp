// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Weapons/BasicPistol.h"
#include "Items/TwoHandedWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	double HitAngle = GetAngleofHit(Hitter->GetActorLocation());
	if (HitAngle >= -120.f && HitAngle < 120.f && bIsBlocking)
	{ // Either Staggger the hitter from blocking a Hit or Get Hit and react or Die
		//if (BlockReactMontage)
		//{
			//PlayBlockSound(ImpactPoint);
		//}
	}
	else
	{
		//ABaseCharacter* HittingChar = Cast<ABaseCharacter>(Hitter);
		//UE_LOG(LogTemp, Warning, TEXT("Equipped Weapon Damage: %f"), HittingChar->EquippedWeapon->GetDamage());
		TakeAHit(ImpactPoint, Hitter);
		if (IsAlive() && Hitter)
		{
			//Getting vectors to calculate Thetha angle from (the direction angle of the attack)
			DirectionalHitReact(Hitter->GetActorLocation());
		}
		else Die();
	}

}

void ABaseCharacter::TakeAHit(const FVector& ImpactPoint, AActor* Hitter)
{
	ABaseCharacter* HittingChar = Cast<ABaseCharacter>(Hitter);
	UE_LOG(LogTemp, Warning, TEXT("Equipped Weapon Damage: %f"), HittingChar->EquippedWeapon->GetDamage());
	UGameplayStatics::ApplyDamage(this, HittingChar->EquippedWeapon->GetDamage(), GetInstigator()->GetController(), this, UDamageType::StaticClass());
	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::Attack() 
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::Block()
{
	IsBlockActive = true;

}

void ABaseCharacter::EndBlock()
{
}

void ABaseCharacter::Die_Implementation()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();

}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	/*
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point To the enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	// This is not a normalised vector, so we need to normalise it by using .GetSafeNormal
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Getting the Dot Product ( Forward * ToHit = |Forward| |ToHit| * cos(theta) )
	// Forward and ToHit are already normalised so they equal 1. Forward * ToHit = cos(theta)
	// So below is equal to cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	// Now we need to get Arc Cos (inverse cos) to get rid of the Cos and get value for theta
	double Theta = FMath::Acos(CosTheta);

	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// if cross product point down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}
	*/
	double Theta = GetAngleofHit(ImpactPoint);
	UE_LOG(LogTemp, Warning, TEXT("MyDouble: %f"), Theta);
	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("FromFrontPlayed"));
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	FString MyFNameString = Section.ToString();

	// Use the UE_LOG macro to print the FString to the log
	UE_LOG(LogTemp, Warning, TEXT("MyFName: %s"), *MyFNameString);

	PlayHitReactMontage(Section);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::PlayBlockSound(const FVector& ImpactPoint)
{
	if (BlockSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BlockSound, ImpactPoint);
	}
}

void ABaseCharacter::PlayParrySound(const FVector& ImpactPoint)
{
	if (ParrySound)
	{
		UE_LOG(LogTemp, Warning, TEXT("PLAY PARRY SOUND!!!!"))
		UGameplayStatics::PlaySoundAtLocation(this, ParrySound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReciveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ABaseCharacter::SelectComboSection(UAnimMontage* Montage, const TArray<FName>& SectionNames, int32& Counter)
{
	if (IsAttackSaved == false)
	{
		Counter = 0;
	}
	// Play Attack montage that is equal to Couter and increment counter by 1 if it exceeded the array limit then zero it.
	PlayMontageSection(Montage, SectionNames[Counter]);
	Counter = (Counter + 1) % SectionNames.Num(); // Reset to zero if it exceeds the array limit
	IsAttackSaved = false;
	return Counter;
}

int32 ABaseCharacter::PlayShieldComboAttack()
{
	return SelectComboSection(AttackMontage, AttackMontageSections, ComboCounter);
}

int32 ABaseCharacter::PlayHeavyComboAttack()
{
	return SelectComboSection(HeavyAttackMontage, HeavyAttackMontageSections, HeavyComboCounter);
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayHeavyAttackMontage()
{
	return PlayRandomMontageSection(HeavyAttackMontage, HeavyAttackMontageSections);
}

int32 ABaseCharacter::PlayJumpAttackMontage()
{
	return PlayRandomMontageSection(JumpAttackMontage, JumpAttackMontageSections);
}

void ABaseCharacter::PlayShootMontage(FName Name)
{
	PlayMontageSection(ShootMontage, Name);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
	
}

void ABaseCharacter::PlayParryMontage()
{
	PlayMontageSection(ParryMontage, FName("Default"));
}

void ABaseCharacter::PlayDodgeMontage(FName Name)
{
	PlayMontageSection(DodgeMontage, Name);
}

void ABaseCharacter::PlayHitReboundMontage()
{
	PlayMontageSection(HitReboundMontage, FName("Default"));
}

void ABaseCharacter::PlayBlockMontage()
{
	PlayMontageSection(BlockMontage, FName("Default"));
}

void ABaseCharacter::PlayCircleAroundMontage()
{
	PlayRandomMontageSection(CircleAroundMontage, CircleAroundMontageSections);
}

void ABaseCharacter::PlayConsumableMontage()
{
	PlayMontageSection(ConsumableMontage, FName("Default"));
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::SpawnDefaultWeapon(FName SocketName)
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), SocketName, this, this);
		EquippedWeapon = DefaultWeapon;
		EquippedWeapon->Tags.Add(FName("EnemyWeapon"));
	}
}

void ABaseCharacter::SpawnDefaultPistol(FName SocketName)
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		ABasicPistol* Pistol = World->SpawnActor<ABasicPistol>(PistolClass);
		Pistol->Equip(GetMesh(), SocketName, this, this);
		EquippedPistol = Pistol;
	}
}

double ABaseCharacter::GetAngleofHit(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point To the enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	// This is not a normalised vector, so we need to normalise it by using .GetSafeNormal
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Getting the Dot Product ( Forward * ToHit = |Forward| |ToHit| * cos(theta) )
	// Forward and ToHit are already normalised so they equal 1. Forward * ToHit = cos(theta)
	// So below is equal to cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	// Now we need to get Arc Cos (inverse cos) to get rid of the Cos and get value for theta
	double Theta = FMath::Acos(CosTheta);

	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// if cross product point down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	return Theta;
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();
	
	// Offsetting the vector of Combat Target Location
	FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector OffsetVector(0.0f, 0.0f, WarpTargetOffset_Z);
	CombatTargetLocation = CombatTargetLocation + OffsetVector;

	const FVector Location = GetActorLocation();
	// Vector of how far is the attacker from its target
	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();

	TargetToMe *= WarpTargetDistance;
	// Vector at the combat target but pushed toward attacker by WarpTargetDistance
	return CombatTargetLocation + TargetToMe;

}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	else return FVector();
}


bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::AttackEnd()
{
	IsBlockActive = false;
	IsParryActive = false;
	//EquippedWeapon->SetBasicDamage();
}

void ABaseCharacter::ResetAttackDamage()
{
	EquippedWeapon->SetBasicDamage();
}

void ABaseCharacter::DodgeEnd()
{
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	// if we have an EquippedWeapon and Weapon is not a null pointer  we set collision enabled for the  weapon box
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

