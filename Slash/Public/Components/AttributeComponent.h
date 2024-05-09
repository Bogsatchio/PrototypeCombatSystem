// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h" // This is from engine folder
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);
	void SprintDrainStamina(float DeltaTime);
protected:

	virtual void BeginPlay() override;

private:
	// Current Health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	//Current Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaDodgeCost = 12.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaAttackCost = 20.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 3.5;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float SprintStaminaDrainRate = 15.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;

// public getters and setters
public:
	void ReciveDamage(float Damage);

	void Heal(float HealingPoints);

	void UseStamina(float StaminaCost);

	float GetHealthPercent();

	float GetStaminaPercent();

	bool IsAlive();

	void AddSouls(int32 NumberOfSouls);
	void AddGold(int32 AmountOfGold);
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetStaminaCost() const { return StaminaCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetStaminaDodgeCost() const { return StaminaDodgeCost; }
	FORCEINLINE float GetStaminaAttackCost() const { return StaminaAttackCost; }

	void SetStaminaCost(float Cost);

};
