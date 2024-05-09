// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/Treasure.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	//Creating Geometry Object and setting it as a rootcomponent and adding Vollision settings
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);

	// Creating Sphere over the Item for collision in smaller breakables
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sphere"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetGenerateOverlapEvents(true);
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	//Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);



}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	// template function so we have to pass in the type in the <>. However we want to pass in a blueprint class that posess a mesh set in UE5.
	// We need to use UClass functionality to get the blueprint class spawned (TreasureClass variable)
	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		// Specyfying Location of the spawn with Z offset to above
		FVector Location = GetActorLocation();
		Location.Z += 75.f;

		// Maximum indesx of the array is number of all elements minus 1. Random Selection between 0 and this max
		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
		GetWorld()->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());
	}


}

