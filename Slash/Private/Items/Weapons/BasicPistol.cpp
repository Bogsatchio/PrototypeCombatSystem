// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/BasicPistol.h"
#include "Kismet/KismetSystemLibrary.h"

ABasicPistol::ABasicPistol()
{
	IsPistol = true;
}

void ABasicPistol::FireTrace(AActor* CombatTarget, FHitResult& BoxHit)
{
	
	const FVector Start = this->GetOwner()->GetActorLocation();
	//FVector ShootingDirection = (CombatTarget->GetActorLocation() - Start).GetSafeNormal();
	FVector End;
	if (CombatTarget)
	{
		End = (CombatTarget->GetActorLocation() - Start).GetSafeNormal() * PistolRange + Start;
	}
	else
	{
		End = this->GetOwner()->GetActorForwardVector() * PistolRange + Start;
	}
	TArray<AActor*> ActorsToIgnore; // storing multiple AActor objects
	if (this->GetOwner()) ActorsToIgnore.Add(this->GetOwner());

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	UKismetSystemLibrary::SphereTraceSingleForObjects(this, Start, End, FireTargetingExtent,
		ObjectTypes, false, ActorsToIgnore, bShowDebugBox ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit, true);
	UE_LOG(LogTemp, Warning, TEXT("Hit Actor's Name: %s"), BoxHit.GetActor());
}

void ABasicPistol::FirePistol(AActor* CombatTarget)
{
	FHitResult BoxHit;
	FireTrace(CombatTarget, BoxHit);
	if (BoxHit.GetActor())
	{
		// Here Activates the Bullet Functionalities
		if (ActorIsSameType(BoxHit.GetActor())) return;

		FString AcName = BoxHit.GetActor()->GetName();
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *AcName);
	}
}
