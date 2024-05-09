// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "HighTierEnemy.generated.h"


class UAnimMontage;

UCLASS()
class SLASH_API AHighTierEnemy : public AEnemy
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(EditAnywhere, Category = "Combat | AttackParameter")
	float BlockInsensitiveAttackPercentage = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Combat | AttackParameter")
	float UnblockableAttackPercentage = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* BlockInsensitiveAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* UnblockableAttackMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> BlockInsensitiveAttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> UnblockableAttackMontageSections;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Deffensive")
	float PercentOfNoHitReact = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Deffensive")
	float PercentOfIntelligentBlock = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Deffensive")
	float PercentOfIntelligentDodges = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Offensive")
	float PercentOfAttackAfterGettingHit = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Offensive")
	float PercentOfCounterAttacks = 0.1f;

	virtual void HitRebound_Implementation(AActor* Hitter) override;
	virtual int32 PlayBlockInsensitiveAttackMontage();
	virtual int32 PlayUnblockableAttackMontage();
	virtual void AttackRandomizer(float RandomNumber2) override;
	virtual void AttackEnd() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	void CheckCombatTarget() override;
	virtual void LoseInterest() override;
	void EnableIntelligentBlock();

	void TakeHitNoHitReact(const FVector& ImpactPoint, AActor* Hitter);

	IHitInterface* HitInterfaceCombatTarget = Cast<IHitInterface>(CombatTarget);
	FTimerHandle IntelligentBlockTimer;

	bool bAttackIgnoreBlock = false;
	bool bImmuneToHitReact = false;
	bool bCanCheckIntelligentBlock = true;
	//bool bAttackUnblockable = false;

};
