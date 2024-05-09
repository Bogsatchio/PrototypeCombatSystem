// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

//Forward Declarations of types
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UTargettingComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class Soul;
class UAnimMontage;
class USlashOverlay;
class ATreasure;
class ATwoHandedWeapon;
class ABasicShield;



UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	virtual void HitRebound_Implementation(AActor* Hitter) override;

	virtual void SetOverlappingItem(AItem* Item) override;

	virtual void AddSouls(ASoul* Soul) override;

	virtual void AddGold(ATreasure* Treasure) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTargettingComponent* TargeterSystem;

	UPROPERTY(VisibleAnywhere)
	int NumHealingPotions = 3;

	UPROPERTY(EditAnywhere, Category = Combat) // TEMPORARY
		float HealthPotionStrength = 40.f;

protected:
	virtual void BeginPlay() override;

	void CheckCharacterSpeed();

	// Creating Input Action that will correspond with those made in Unreal Engine for Echo

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* SlashContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* BlockAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* TargetAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ConsumableAction;

	

	/**
	  Callbacks for input
	*/

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	void EKeyPressed();
	void EquipOneHanded(AWeapon* OverlappingWeapon);
	void EquipShield(ABasicShield* OverlappingShield);
	void EquipTwoHanded(ATwoHandedWeapon* OverlappingWeapon_2);
	void StartSprint();
	void StopSprint();
	virtual void Attack() override; // From BaseCharacter Class
	virtual void DefensiveAction();
	virtual void Block() override;
	virtual void AimToShoot();
	virtual void EndBlock() override;
	virtual void Parry();
	void NormalAttack();
	void HeavyAttack();
	void Dodge();
	void AcquireTarget();
	void UseCurrentConsumable();

	UFUNCTION(BlueprintCallable)
	virtual void ParryEnd();

	void DrainStamina(float Usage);
	bool HasStamina();
	virtual int32 SelectComboSection(UAnimMontage* Montage, const TArray<FName>& SectionNames, int32& Counter) override;
	virtual int32 PlayNoShieldComboAttack();

	bool bIsSprinting = false;
	/**
	  Play Montage functions. (BlueprintCallable Execution from anim notify)
	*/

	/* <Jump Attack> */
	//UPROPERTY(EditDefaultsOnly, Category = Combat)
	//UAnimMontage* JumpAttackMontage;

	//UPROPERTY(EditAnywhere, Category = Combat)
	//TArray<FName> JumpAttackMontageSections;

	//int32 PlayJumpAttackMontage();
	/* </Jump Attack> */

	void PlayEquipMontage(FName SectionName);
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;

	UFUNCTION(BlueprintCallable)
	virtual void Shoot();

	UFUNCTION(BlueprintCallable)
	void ActivateConsumablesEffect();

	virtual void Die_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void ContinueAttack();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void PistolDisarm();

	UFUNCTION(BlueprintCallable)
	void PistolArm();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UFUNCTION(BlueprintCallable)
	void IntouchabilityOn();

	UFUNCTION(BlueprintCallable)
	void IntouchabilityOff();



private:
	/* HUD Components*/
	void InitializeSlashOverlay(APlayerController* PlayerController);

	void SetHUDHealth();

	/* </> HUD Components*/

	// Declaring a variable that will hold the value for current Character State and initializing it to Unequipped
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	// Declaring variable that holds value for another enum Action State
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	EAttackType AttackType = EAttackType::EAT_None;

	/* Character Components*/

	//remember to tick Use Pawn Controller Rotation in Camera Settings of CameraBoom to have camera rotate with mouse

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	 UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	// declaring a variable that stores the info on item that is overlaping with the Character
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* NoShieldAttackMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> NoShieldAttackMontageSections;

	UPROPERTY()
	double CharacterSpeed;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

	UPROPERTY(EditAnywhere)
	float NoShieldSprintSpeed = 900;

	UPROPERTY(EditAnywhere)
	float WithShieldSprintSpeed = 780;



public:  //Getter and Setter for overlaping with the Item
	// FORCEINLINE is a macro that forces the function to be read as inline function by the compiler (speeds up the execution)

	// Getter for the Character State that is private variable. We will use it in AnimInstance.
	// This function doesn't change anything so we should make it a const function
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	void SetCombatTarget(AActor* Target);

};
