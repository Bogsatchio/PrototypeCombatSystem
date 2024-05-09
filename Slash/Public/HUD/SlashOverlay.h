// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);
	void SetConsumable(int32 Potions);



private:
	// Metaspecifier BindWidget is used to link variable with the object in UE5 with the same name
	UPROPERTY(meta = (BindWidget)) // This will bind the variable in C++ with the one in blueprint but the HAVE TO have the same name
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget)) // This will bind the variable in C++ with the one in blueprint but the HAVE TO have the same name
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget)) // This will bind the variable in C++ with the one in blueprint but the HAVE TO have the same name
	UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget)) // This will bind the variable in C++ with the one in blueprint but the HAVE TO have the same name
	UTextBlock* SoulsText;

	UPROPERTY(meta = (BindWidget)) // This will bind the variable in C++ with the one in blueprint but the HAVE TO have the same name
	UTextBlock* ConsumableNum;
};
