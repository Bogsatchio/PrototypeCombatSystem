// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"
#include "Characters/SlashCharacter.h"
#include "Components/SphereComponent.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// template function to create a mesh and stores it in a pointer variable
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//reassigning RootComponent of the class to the mesh
	RootComponent = ItemMesh;

	// Creating Sphere over the Item
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	// Creating Niagra particle Embers Effect over Item
	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	ItemEffect->SetupAttachment(GetRootComponent());


}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// variable that is a Pointer to GetWorld function that gets us the world of the level
	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	//DRAW_POINT(GetActorLocation());
	
	
	//Binding callback functions to the delegate 
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::EndSphereOverlap);

	
}

float AItem::TransformedSin(float Value)
{
	return Amplitude * FMath::Sin(Value * TimeConstant);
}

float AItem::TransformedCos(float Value)
{
	return Amplitude * FMath::Cos(Value * TimeConstant);
}

//When this function gets called it passes in the information about the overlap events. So that the Item itself doesn't have to know anything about it.
void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Casting Other Actor input to PickupInterface and checking if its not a nullptr and only then activating the setter function of Character.
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingItem(this);
	}

}

void AItem::EndSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Same logic as with setting Overlap Item but we are setting it to nullptr
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingItem(nullptr);
	}
}

void AItem::SpawnPickupSystem()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
	}
}

void AItem::SpawnPickupSound()
{
	if (PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, PickupSound, GetActorLocation());
	}

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	// Using Sin function from FMath module of UE. Amplitude is scaling the output of sin so the range of motion and TimeConstant is determining how fast the change occurs so the speed of motion
	//float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);

	float DeltaX = TransformedSin(RunningTime);
	float DeltaY = TransformedCos(RunningTime);
	float DeltaZ = TransformedSin(RunningTime);

	// Hovering and moving ability

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0, 0, DeltaZ));
		AddActorWorldRotation(FRotator(0.f, Rotation, 0.f));
	}
	
	//AddActorWorldOffset(FVector(DeltaX, DeltaY, DeltaX));
	// Rotating ability
	//AddActorWorldRotation(FRotator(0.f, Rotation, 0.f));

	//DRAW_SPHERE_SingleFrame(GetActorLocation());

}

