// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ASoul : public AItem
{
	GENERATED_BODY()
	

protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void SetDriftingTarget(AActor* OtherActor);
	void DelayedSetDriftingTarget(AActor* OtherActor);

	void ActivatePickup(AActor* OtherActor);
	bool InRangeOf (AActor* Target, double Radius);

	void CheckforDrifting(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Combat")
	double DriftRadius = 1000.f;

	double PickUpRadius = 50.f;


	UPROPERTY(VisibleAnywhere)
	AActor* PlayerCharacter;
	FVector PlayerLocation;


private:
	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 Souls;

	FTimerHandle Timer;

	UPROPERTY(VisibleAnywhere)
	bool bCanDrift = false;

public:
	virtual void Tick(float DeltaTime) override;
	ASoul();
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }
};
