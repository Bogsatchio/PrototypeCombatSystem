// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController;
class UAnimMontage;
class UPawnSensingComponent;
class UWidgetComponent;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	
	AEnemy();

	void EnableLockOnWidget();
	void DisableLockOnWidget();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	virtual void HitRebound_Implementation(AActor* Hitter) override;

	/** </IHitInterface> */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	bool bIsATarget = false;   // Determines whether the targetting dot is displayed

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* LockOnWidget;

protected:
	/** <AActor> */
	virtual void BeginPlay() override;


	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	void SpawnSoul();
	bool CanAttack(); // Should be an override from BaseCharacter, might add later
	bool CanJumpAttack();
	bool CanDodge();
	virtual void Attack() override;
	void PerformCombatMovement(float RandomNumber);
	virtual void AttackRandomizer(float RandomNumber2);
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;
	virtual void Block() override;
	virtual void Dodge();
	virtual void CircleAround();
	/** </ABaseCharacter> */

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	//EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class UUserWidget> LockOnWidgetClass;

	UFUNCTION(BlueprintCallable)
	virtual void BeginBlock();

	UFUNCTION(BlueprintCallable)
	virtual void CombackBlockRandomizer();

	UFUNCTION(BlueprintCallable)
	virtual void CombackAttackRandomizer();


//private:

	/** AI Behavior */
	void InitializeEnemy();
	virtual void CheckCombatTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();

	void HideHealthBar();
	void ShowHealthBar();
	virtual void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsInsideJumpAttackRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool InTargetRange(AActor* Target, double Radius);
	void ClearPatrolTimer();
	//void SpawnDefaultWeapon();
	
	void JumpAttackRandomizer();
	void StartAttackTimer();
	void ClearAttackTimer();
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); //Callback for OnPawnSeen in UPawnSensingComponent

	/** Components */

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	//UPROPERTY(EditAnywhere)
	//TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY()
	AAIController* EnemyController;

	/** Combat */

	FTimerHandle AttackTimer;
	FTimerHandle JumpAttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AttackRadius = 190.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double JumpAttackRadius = 320.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AcceptanceRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 0.5f;



	UPROPERTY(EditAnywhere, Category = "Combat | MainCombatParameter")
	float PercentOfAttacks = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Combat | MainCombatParameter")
	float PercentofCombatMovement = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Combat | MainCombatParameter")
	float PercentOfBlocks = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Offensive")
	float PercentOfLightAttacks = 0.75f;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Deffensive")
	float PercentOfDodges = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Deffensive")
	float PercentOfComebackBlock = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Offensive")
	float PercentOfComebackAttack = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Offensive")
	float PercentOfJumpAttacks = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Combat | SecondaryCombatParameter | Offensive")
	float PercentOfStaggerPunishment = 0.95f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 8.f;

	/** Patrolling */

	FTimerHandle PatrolTimer;

	UPROPERTY()
	double PatrolRadius = 200.f;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*>PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASoul> SoulClass;




};
