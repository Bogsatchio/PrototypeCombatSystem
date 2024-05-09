// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"

void ASoul::Tick(float DeltaTime)
{
	if (PlayerCharacter && bCanDrift)
	{
		CheckforDrifting(DeltaTime);
	}
}

ASoul::ASoul()
{
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//SetDriftingTarget(OtherActor);
	if (bCanDrift == false || PlayerCharacter == nullptr)
	{
		Sphere->SetSphereRadius(50);
		bCanDrift = true;
		DelayedSetDriftingTarget(OtherActor);
	}
	//GetWorldTimerManager().SetTimer(Timer, [this, OtherActor]() {ActivatePickup(OtherActor);}, 4, false);
}

void ASoul::SetDriftingTarget(AActor* OtherActor)
{
	if (PlayerCharacter == nullptr)
	{
		Sphere->SetSphereRadius(50);
		//First To PickupInterface then to Actor to ensure that only SlashCharacter can pick it up
		IPickupInterface* Pickup = Cast<IPickupInterface>(OtherActor);
		PlayerCharacter = Cast<AActor>(Pickup);
		//bCanDrift = true;
	}
}

void ASoul::DelayedSetDriftingTarget(AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Delayed Drifting Triggered"));
	GetWorldTimerManager().SetTimer(Timer, [this, OtherActor]() {SetDriftingTarget(OtherActor); }, 1, false);
}

void ASoul::ActivatePickup(AActor* OtherActor)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddSouls(this);

		SpawnPickupSystem();
		SpawnPickupSound();
		Destroy();
	}
}

bool ASoul::InRangeOf(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void ASoul::CheckforDrifting(float DeltaTime) // Dodaæ jako argument Character do którego leci
{
		//Check if is in range for a drift and then drifts if is in range for pickup it picks up
		if (InRangeOf(PlayerCharacter, DriftRadius) && !InRangeOf(PlayerCharacter, PickUpRadius))
		{
			PlayerLocation = PlayerCharacter->GetActorLocation();
			FVector CurrentLocation = GetActorLocation();

			// Calculate the direction from the item to the player
			FVector Direction = PlayerLocation - CurrentLocation;
			Direction.Normalize();

			// Calculate the new location based on the direction and movement speed
			FVector NewLocation = CurrentLocation + (Direction * 250 * DeltaTime);

			// Move the item towards the player
			SetActorLocation(NewLocation);
		}
		else if (InRangeOf(PlayerCharacter, PickUpRadius))
		{
			UE_LOG(LogTemp, Warning, TEXT("Picking Up the soul"));
			ActivatePickup(PlayerCharacter);
			//DelayedActivatePickup(PlayerCharacter);
			bCanDrift = false;

			//ActivatePickup(PlayerCharacter);
		}

}



