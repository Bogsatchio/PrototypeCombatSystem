// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

// forward declaration of brakable geometry collection
class UGeometryCollectionComponent;
class UCapsuleComponent;


UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

	// overriding get hit function from IHitInterface to create the reaction to getting hit
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

private:
	

	// UClass or TSubclassOf<class> variable that can be set in blueprint and represent another blueprint or C++ Class. 
	// We will set BP_Treasure here.
	UPROPERTY(EditAnywhere)
		// class ATreasure is forward declared here insead of the top. It is basically an array of Atreasure SubclassOf elements
		TArray < TSubclassOf<class ATreasure>> TreasureClasses;
	
	
	

};
