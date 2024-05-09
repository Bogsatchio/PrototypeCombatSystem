// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/Weapon.h"
#include "BasicPistol.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ABasicPistol : public AWeapon
{
	GENERATED_BODY()

public:
	ABasicPistol();

	void FireTrace(AActor* CombatTarget, FHitResult& BoxHit);

	void FirePistol(AActor* CombatTarget);

private:

	UPROPERTY(EditAnywhere, Category = "Tracing")
	float PistolRange = 1500;

	UPROPERTY(EditAnywhere, Category = "Tracing")
	float FireTargetingExtent = 100;
	
};
