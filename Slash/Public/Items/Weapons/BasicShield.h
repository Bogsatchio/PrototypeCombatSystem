// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/Weapon.h"
#include "BasicShield.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ABasicShield : public AWeapon
{
	GENERATED_BODY()
	
public:
	ABasicShield();
	void BlockCollision();
	UStaticMeshComponent* ReturnMesh();
};
