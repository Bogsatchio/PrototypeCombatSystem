// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"


class AWeapon;
class ABasicShield;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	void TakeAHit(const FVector& ImpactPoint, AActor* Hitter);

	/* Combat */
	virtual void Attack();
	virtual void Block();
	virtual void EndBlock();

	UFUNCTION(BlueprintNativeEvent) // NativeEvent needs _Implementation added
	void Die();

	virtual void HandleDamage(float DamageAmount);
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void PlayBlockSound(const FVector& ImpactPoint);
	void PlayParrySound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

	/* Montage */
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	virtual int32 SelectComboSection(UAnimMontage* Montage, const TArray<FName>& SectionNames, int32& Counter);
	//virtual int32 PlayNoShieldComboAttack();
	virtual int32 PlayShieldComboAttack();
	virtual int32 PlayHeavyComboAttack();
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayHeavyAttackMontage();
	virtual int32 PlayJumpAttackMontage();
	virtual void PlayShootMontage(FName Name);
	virtual int32 PlayDeathMontage();
	virtual void PlayParryMontage();
	virtual void PlayDodgeMontage(FName Name);
	virtual void PlayHitReboundMontage();
	virtual void PlayBlockMontage();
	virtual void PlayCircleAroundMontage();
	void PlayConsumableMontage();
	void StopAttackMontage();
	void DisableCapsule();
	void SpawnDefaultWeapon(FName SocketName);
	void SpawnDefaultPistol(FName SocketName);

	double GetAngleofHit(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetOffset_Z = -10.f;

	UPROPERTY(VisibleAnywhere)
	int32 ComboCounter = 0;

	UPROPERTY(VisibleAnywhere)
	int32 HeavyComboCounter = 0;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float HeavyDamageMultiplicator = 2.f;

	UPROPERTY(VisibleAnywhere)
	bool IsAttackSaved = false;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABasicPistol> PistolClass;

	bool bHasShield = false;
	bool bIsBlocking = false;

	bool IsAlive();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void ResetAttackDamage();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	ABasicPistol* EquippedPistol;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	ABasicShield* EquippedShield;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* BlockReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* StaggerMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* HitReboundMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* CircleAroundMontage;

private:

	UPROPERTY(EditAnywhere, Category = Combat )
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* BlockSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* ParrySound;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* HitParticles;

	/**
	  Animation montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* JumpAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* ShootMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* BlockMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* ParryMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* ConsumableMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> HeavyAttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> CircleAroundMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> JumpAttackMontageSections;

public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }

};
