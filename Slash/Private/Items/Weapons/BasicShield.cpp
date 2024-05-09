// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/BasicShield.h"

ABasicShield::ABasicShield()
{
	//ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	//ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
}

void ABasicShield::BlockCollision()
{
	;
	//ItemMesh->OnComponentHit.AddDynamic()
}

UStaticMeshComponent* ABasicShield::ReturnMesh()
{
	return ItemMesh;
}
