// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/Components/RotateCharacter.h"
#include "GameFramework/Character.h"
#include "Characters/SlashCharacter.h"
#include "Math/UnrealMathUtility.h"

void URotateCharacter::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("NotifyTick firing!!!"));
	//Character = Cast<ACharacter>(MeshComp->GetOwner());
	Char = Cast<ASlashCharacter>(MeshComp->GetOwner());

	if (Char)
	{
		FVector Dir = Char->GetLastMovementInputVector();

		// Checks if Character Has Movement Input. When It Has Input then rotate character according to the input. When it doesn't have then get the current rotation
		if (Dir.IsNearlyZero(0.005))
		{
			Char->SetActorRotation(Char->GetActorRotation());
		}
		else
		{

			FRotator TargetRotation = FRotationMatrix::MakeFromX(Dir).Rotator();

			FRotator NewRotation = FMath::RInterpConstantTo(Char->GetActorRotation(), TargetRotation, FrameDeltaTime, 520.f);
			Char->SetActorRotation(NewRotation);
		}
	}
}
