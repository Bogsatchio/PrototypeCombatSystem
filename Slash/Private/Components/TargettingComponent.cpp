// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TargettingComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h" // Check if can be deleted
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/HitInterface.h"
#include "Enemy/Enemy.h"

// Sets default values for this component's properties
UTargettingComponent::UTargettingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTargettingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UTargettingComponent::EnableLockOn()
{
	FindTarget();
	bIsTargetting = true;
	if (Hostile)
	{
		Hostile->EnableLockOnWidget();
		
	}
}

void UTargettingComponent::DisableLockOn()
{
	TargetActor = nullptr;

	bIsTargetting = false;
	if (Hostile)
	{
		Hostile->DisableLockOnWidget();
	}
	ACharacter* Player = Cast<ACharacter>(this->GetOwner());
	Player->GetCharacterMovement()->bOrientRotationToMovement = true;

}

bool UTargettingComponent::CanBeTargeted(AActor* Target)
{
	// Can be targeted if is an enemy is not dead and is no further than 1500 cm
	Hostile = Cast<AEnemy>(Target);
	if (Hostile)
	{
		//if (!bIsTargetting) Hostile->bIsATarget = false;
		//Hostile->bIsATarget = true;
		float Distance = FVector::Distance(Hostile->GetActorLocation(), this->GetOwner()->GetActorLocation());

		if (Hostile->EnemyState == EEnemyState::EES_Dead || Distance > 1500.f)
		{
			//Hostile->bIsATarget = false;
			return false;
		}
		else return true;
	}
	else return false;

}

void UTargettingComponent::FindTarget()
{
	// This function performs a trace and finds and sets TargetActor
	FHitResult BoxHit;
	TraceForTarget(BoxHit);

	if (BoxHit.GetActor())
	{
		if (CanBeTargeted(BoxHit.GetActor()))
		{
			TargetActor = BoxHit.GetActor();
			//bIsTargetting = true;
		}

	}
	OrientPlayerToTarget();


}

void UTargettingComponent::ChangeTarget(float AxisVector)
{
	FHitResult BoxHit;
	ChangeTargetTrace(BoxHit, AxisVector);

	if (BoxHit.GetActor())
	{
		Hostile->DisableLockOnWidget();
		if (CanBeTargeted(BoxHit.GetActor()))
		{
			DisableLockOn();
			TargetActor = BoxHit.GetActor();
			bIsTargetting = true;
			if (Hostile)
			{
				Hostile->EnableLockOnWidget();

			}
		}

	}
	OrientPlayerToTarget();
}

void UTargettingComponent::TraceForTarget(FHitResult& BoxHit)
{
	const FVector Start = this->GetOwner()->GetActorLocation();

	const FVector End = FollowCamera->GetForwardVector() * TargetingDistance + Start;

	TArray<AActor*> ActorsToIgnore; // storing multiple AActor objects
	if (this->GetOwner()) ActorsToIgnore.Add(this->GetOwner());
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		
	UKismetSystemLibrary::SphereTraceSingleForObjects(this, Start, End, TargetingExtent,
		ObjectTypes, false, ActorsToIgnore, bShowDebugBox ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit, true);
		
}

void UTargettingComponent::ChangeTargetTrace(FHitResult& BoxHit, float XLookAxisVector)
{
	if (TargetActor)
	{
		const FVector Start = this->GetOwner()->GetActorLocation();
		FVector End;
		if (XLookAxisVector > 0)
		{
			FRotator Rotation = FRotator(0.0f, 45.0f, 0.0f);
			End = Rotation.RotateVector(FollowCamera->GetForwardVector()) * TargetingDistance + Start;
		}
		else if (XLookAxisVector < 0)
		{
			FRotator Rotation = FRotator(0.0f, -45.0f, 0.0f);
			End = Rotation.RotateVector(FollowCamera->GetForwardVector()) * TargetingDistance + Start;
		}
		TArray<AActor*> ActorsToIgnore; // storing multiple AActor objects
		if (this->GetOwner()) ActorsToIgnore.Add(this->GetOwner());
		ActorsToIgnore.Add(TargetActor);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(ActorsToIgnore);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		UKismetSystemLibrary::SphereTraceSingleForObjects(this, Start, End, TargetingExtent,
			ObjectTypes, false, ActorsToIgnore, bShowDebugBox ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			BoxHit, true);

		
	}
	

}

void UTargettingComponent::UpdateTargetingControlRotation(float FrameDeltaTime)
{
	if (this->GetOwner() && TargetActor)
	{
		//FRotator CharacterRotation = this->GetOwner()->GetActorRotation();
		FRotator CharacterRotation = PlayerController->GetControlRotation();
		//AController* Controller = Cast<ACharacter>(this->GetOwner())->GetController();
		//FRotator CRotation = Controller->GetActorRotation();

		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(this->GetOwner()->GetActorLocation(), TargetActor->GetActorLocation());
		TargetRotation.Roll -= 5;
		TargetRotation.Yaw -= 10;
		FRotator NewRotation = FMath::RInterpConstantTo(CharacterRotation, TargetRotation, FrameDeltaTime, CamRotationSpeed);

		OutputRotation.Roll = CharacterRotation.Roll;
		OutputRotation.Pitch = NewRotation.Pitch;
		OutputRotation.Yaw = NewRotation.Yaw;

		//this->GetOwner()->SetActorRotation(OutputRotation);
		PlayerController->SetControlRotation(OutputRotation);
	}
}

void UTargettingComponent::OrientPlayerToTarget()
{
	if (bShouldRotateCamera)
	{
		ACharacter* Player = Cast<ACharacter>(this->GetOwner());
		Player->bUseControllerRotationYaw = false;
		Player->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		Player->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}


// Called every frame
void UTargettingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateTargetingControlRotation(DeltaTime);
	if (!CanBeTargeted(TargetActor))
	{
		DisableLockOn();
	}
}

