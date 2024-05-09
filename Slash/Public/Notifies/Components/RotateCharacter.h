// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RotateCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API URotateCharacter : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	ACharacter* Character;
	class ASlashCharacter* Char;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	
};
