// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h" // to get slash character that the animation will be cast to
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// initializing SlashCharacter variable by casting Pawn Owner to a character class
	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
	if (SlashCharacter)
	{
		// Assigning the ValueMovement Component to variable
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement();
	}
	

}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacterMovement)
	{
		// Getting XY Vector of characterMovementComponent casted to float
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);

		Direction = this->CalculateDirection(SlashCharacterMovement->Velocity, SlashCharacter->GetActorRotation());

		// Checking if is falling to activate jump animation. IsFalling() is a function form CharacterMovementComponent
		IsFalling = SlashCharacterMovement->IsFalling();

		CharacterState = SlashCharacter->GetCharacterState();

		ActionState = SlashCharacter->GetActionState();

		DeathPose = SlashCharacter->GetDeathPose();
	}

}
