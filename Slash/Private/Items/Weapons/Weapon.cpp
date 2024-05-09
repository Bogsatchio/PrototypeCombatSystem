// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	// Creating a box Component and attaching it to the root of the weapon
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());

	// Setting Collision settings from C++ instead of UE5
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponBox->SetGenerateOverlapEvents(true);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());


}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

// This is a function that is called on pressing the E Key in SlashCharacter.cpp
 void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	 ItemState = EItemState::EIS_Equipped;
	 // Associating Character whose equiping a weapon with a weapon itself
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	AttachMeshToSocket(InParent, InSocketName);
	DisableSphereCollision();

	PlayEquipSound();
	DeactivatingEmbers();
}

 void AWeapon::DeactivatingEmbers()
 {
	 // Deactivating Niagara's EmberEffect
	 if (ItemEffect)
	 {
		 ItemEffect->Deactivate();
	 }
 }

 void AWeapon::DisableSphereCollision()
 {
	 if (Sphere)
	 {
		 Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	 }
 }

 void AWeapon::PlayEquipSound()
 {
	 if (EquipSound)
	 {
		 UGameplayStatics::PlaySoundAtLocation(
			 this,
			 EquipSound,
			 GetActorLocation()
		 );
	 }
 }

 void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
 {
	 // Variable that determines the transform rules of attachment
	 FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	 // Attaching Item component to Character component using function AttachToComponent(Component we attach to, Rules of Attachment, Socket Name)
	 ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
 }

// The function that performs the actual Box trace for the Start to End Components
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) return;
	
	FHitResult BoxHit;
	BoxTrace(BoxHit);


	if (BoxHit.GetActor())
	{
		if (ActorIsSameType(BoxHit.GetActor())) return;
		
		FString AcName = BoxHit.GetActor()->GetName();
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	// Checking if the actor that got hit implements the interface for function that will be used
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());

	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
		// If the Actor is blocking Owner of the Weapon (MainCharacter) will execute HitRebound
		if (HitInterface->IsBlockActive || HitInterface->IsParryActive)
		{
			if (!IsPistol)
			{
				//Interface function EXECUTE_REBOUND() (Hitter Executes Rebound on Block)
				IHitInterface::Execute_HitRebound(GetOwner(), BoxHit.GetActor());
			}
			
		}
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	// Start and end components for the box trace
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore; // storing multiple AActor objects
	ActorsToIgnore.Add(this);
	if (this->GetOwner()) ActorsToIgnore.Add(this->GetOwner());

	// All the actors that got hit already are being added to be ignored. This way we hit particular Actor only once per swing.
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}
	/*
	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxTraceExtent, BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, bShowDebugBox ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit, true);
	IgnoreActors.AddUnique(BoxHit.GetActor());
	*/

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMulti(this, Start, End, BoxTraceExtent, BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, bShowDebugBox ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutHits, true);

	for (const FHitResult& HitResult : OutHits)
	{
		AActor* ActorHit = HitResult.GetActor();
		if (!ActorHit || ActorHit->IsA(AItem::StaticClass()))
		{
			// Ignore actors of the same class
			continue;
		}

		BoxHit = HitResult; // Set BoxHit to the first valid hit result
		IgnoreActors.AddUnique(BoxHit.GetActor());
		return; // Exit the function after finding the first valid hit result
	}



}


