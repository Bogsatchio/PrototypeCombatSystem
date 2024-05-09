// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"



class USphereComponent;
class UNiagaraComponent;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters") // exposes the variable to reflection system. Here we can edit it everywhere in a Blueprint. With EditDefaultsOnly We can edit the variable only in the default blueprint
		// Uproperty for blueprint for Event Graph can't be in private section. BlueprintReadOnly 
		// Category creates a custom category for variable in details panel of UE5.
		float Amplitude = 0.1;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters") //With EditInstanceOnly We can Edit the variable only in an Instance in the worlds
		float TimeConstant = 0.6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
		float Rotation = 0.1f;

	// Exposes a function to blueprint
	UFUNCTION(BlueprintCallable) // Makes it Available in event graph. BlueprintPure is for functions that just takes value and returns other value and doesn't change anything else
		float TransformedSin(float Value);

	UFUNCTION(BlueprintPure)
		float TransformedCos(float Value);

	// declaring callback function that detects overlap
	UFUNCTION() // This will ensure that we can bind the function to the delegate
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declaring callback function that detects the end of overlap
	UFUNCTION()
	virtual void EndSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void SpawnPickupSystem();
	virtual void SpawnPickupSound();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* ItemEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;

private:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) // Visible but not editable variable in a blueprint. VisibleDefaultsOnly just in default blueprint. VisibleAnywhere is also possible
		// We can have Blueprint for eventgraph in private only if we have metaspecifier AllowPrivateAccess
	float RunningTime;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;







};
