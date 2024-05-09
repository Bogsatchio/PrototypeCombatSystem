// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
class UProgressBar;

UCLASS()
class SLASH_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget)) // This will bind the variable in C++ with the one in blueprint but the HAVE TO have the same name
	UProgressBar* HealthBar;


	
};
