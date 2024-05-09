// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TargettingComponent.generated.h"

class UCameraComponent;
class AEnemy;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UTargettingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargettingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TraceForTarget(FHitResult& BoxHit);
	void ChangeTargetTrace(FHitResult& BoxHit, float XLookAxisVector);
	void UpdateTargetingControlRotation(float FrameDeltaTime);
	void OrientPlayerToTarget();
	void FindTarget();
	void ChangeTarget(float AxisVector);
	void EnableLockOn();
	void DisableLockOn();
	bool CanBeTargeted(AActor* Target);
	bool bShouldRotateCamera = true;

	FORCEINLINE bool GetIsTargetting() { return bIsTargetting; }

	UCameraComponent* FollowCamera;

	FRotator OutputRotation;
	//APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this->GetOwner(), 0);

	bool bIsTargetting = false;
	AEnemy* Hostile;

	UPROPERTY(EditAnywhere, Category = "Combat")
	AActor* TargetActor;

private:

	//FVector BoxTraceExtent = FVector(TargetingExtent, TargetingExtent, TargetingExtent);

	//UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Tracing")
	float CamRotationSpeed = 20;

	UPROPERTY(EditAnywhere, Category = "Tracing")
	float TargetingDistance = 800;

	UPROPERTY(EditAnywhere, Category = "Tracing")
	float TargetingExtent = 500.f;

	UPROPERTY(EditAnywhere, Category = "Tracing")
	bool bShowDebugBox = true;

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
};
