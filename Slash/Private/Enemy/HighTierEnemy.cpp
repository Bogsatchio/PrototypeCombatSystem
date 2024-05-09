// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/HighTierEnemy.h"

void AHighTierEnemy::HitRebound_Implementation(AActor* Hitter)
{
	// Checking if AttackTarget that is implementing HitInterface and extracting information whether is blocking or parrying
	IHitInterface* HitInterfaceTarget = Cast<IHitInterface>(Hitter);
	// When enemy performs Attack that ignores block and Target is blocking HitRebound is not played 
	if (bAttackIgnoreBlock && HitInterfaceTarget->IsBlockActive) return;
	Super::HitRebound_Implementation(Hitter);
}

int32 AHighTierEnemy::PlayBlockInsensitiveAttackMontage()
{
	return PlayRandomMontageSection(BlockInsensitiveAttackMontage, BlockInsensitiveAttackMontageSections);
}

int32 AHighTierEnemy::PlayUnblockableAttackMontage()
{
	return PlayRandomMontageSection(UnblockableAttackMontage, UnblockableAttackMontageSections);
}

void AHighTierEnemy::AttackRandomizer(float RandomNumber2)
{
	float RandomNumber = FMath::FRandRange(0.0f, 1.0f);
	if (RandomNumber < UnblockableAttackPercentage)
	{
		bImmuneToHitReact = true;
		bAttackUnblockable = true;
		EnemyState = EEnemyState::EES_Engaged;
		PlayUnblockableAttackMontage();
	}
	else if (RandomNumber < UnblockableAttackPercentage + BlockInsensitiveAttackPercentage)
	{
		bImmuneToHitReact = true;
		bAttackIgnoreBlock = true;
		EnemyState = EEnemyState::EES_Engaged;
		PlayBlockInsensitiveAttackMontage();
	}
	else
	{
		Super::AttackRandomizer(RandomNumber2);
	}
}

void AHighTierEnemy::AttackEnd()
{
	Super::AttackEnd();
	bAttackIgnoreBlock = false;
	bAttackUnblockable = false;
	bImmuneToHitReact = false;
}

void AHighTierEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (EnemyState != EEnemyState::EES_Block && bImmuneToHitReact)
	{
		TakeHitNoHitReact(ImpactPoint, Hitter);
	}
	else if (EnemyState != EEnemyState::EES_Block)
	{
		float RandomNumber = FMath::FRandRange(0.0f, 1.0f);
		if (RandomNumber < PercentOfNoHitReact)
		{
			TakeHitNoHitReact(ImpactPoint, Hitter);
		}
		else
		{
			Super::GetHit_Implementation(ImpactPoint, Hitter);
		}
	}
	else
	{
		Super::GetHit_Implementation(ImpactPoint, Hitter);
	}
	float RandomNumber = FMath::FRandRange(0.0f, 1.0f);
	float RandomNumber2 = FMath::FRandRange(0.0f, 1.0f);
	if (RandomNumber < PercentOfAttackAfterGettingHit && IsAlive() && EnemyState == EEnemyState::EES_HitReaction)
	{ // Szansa na wykonanie ataku po otrzymaniu obra¿eñ
		AttackRandomizer(RandomNumber2);
	}
	if (RandomNumber < PercentOfCounterAttacks && IsAlive() && EnemyState == EEnemyState::EES_Block)
	{ // Szansa na wykonanie ataku po zablokowaniu (TUTAJ MO¯NA SPECJALNY MONTA¯ KONTRY DAÆ)
		AttackRandomizer(RandomNumber2);
	}
	if (HitInterfaceCombatTarget) // Wy³¹czenie przekazywania informacji o ataku bo atak ju¿ wyl¹dowa³
	{
		HitInterfaceCombatTarget->IsPlayerAttacking = false;
	}
}

void AHighTierEnemy::CheckCombatTarget()
{
	if (!HitInterfaceCombatTarget && CombatTarget)
	{
		HitInterfaceCombatTarget = Cast<IHitInterface>(CombatTarget);
	}
	
	if (IsInsideJumpAttackRadius() && EnemyState != EEnemyState::EES_Dead && HitInterfaceCombatTarget && HitInterfaceCombatTarget->IsPlayerAttacking && CanDodge() && EnemyState != EEnemyState::EES_Block && bCanCheckIntelligentBlock)
	{
		bCanCheckIntelligentBlock = false; // disabling intelligent block so that the code here won't execute every frame and after one second enabling it again
		GetWorldTimerManager().SetTimer(IntelligentBlockTimer, this, &AHighTierEnemy::EnableIntelligentBlock, 1.5);
		float RandomNumber = FMath::FRandRange(0.0f, 1.0f);
		if (RandomNumber < PercentOfIntelligentBlock)
		{
			UE_LOG(LogTemp, Warning, TEXT("INTELLIGENT BLOCK"));
			Block();
			//bCanCheckIntelligentBlock = false; // disabling intelligent block so that the code here won't execute every frame and after one second enabling it again
			GetWorldTimerManager().SetTimer(IntelligentBlockTimer, this, &AHighTierEnemy::EnableIntelligentBlock, 1.5);
		}
		else if (RandomNumber < PercentOfIntelligentDodges)
		{
			Dodge();
			//bCanCheckIntelligentBlock = false; // disabling intelligent block so that the code here won't execute every frame and after one second enabling it again
			GetWorldTimerManager().SetTimer(IntelligentBlockTimer, this, &AHighTierEnemy::EnableIntelligentBlock, 1.5);
		}
		else
		{
			Super::CheckCombatTarget();
		}

	}
	else
	{
		Super::CheckCombatTarget();
	}
}

void AHighTierEnemy::LoseInterest()
{
	HitInterfaceCombatTarget = nullptr;
	Super::LoseInterest();
}

void AHighTierEnemy::EnableIntelligentBlock()
{
	bCanCheckIntelligentBlock = true;
}

void AHighTierEnemy::TakeHitNoHitReact(const FVector& ImpactPoint, AActor* Hitter)
{
	TakeAHit(ImpactPoint, Hitter);
	CombatTarget = Hitter;
	ShowHealthBar();
	if (!IsAlive()) Die();
}
