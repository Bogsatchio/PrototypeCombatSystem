// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"



class USoundBase;
class UBoxComponent;

/**
 * 
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	// constructor function
	AWeapon();

	// Equip function that needs Component we are attaching item to and socket
	virtual void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	void DeactivatingEmbers();

	void DisableSphereCollision();

	void PlayEquipSound();

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);


	TArray<AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& BoxHit);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f, 5.f,50.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowDebugBox = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float BasicDamage = 20.f;

	//UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;



	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	bool IsPistol = false;


private:

	// !!! Weapon Box as well as Start and End Trace should be in protected for use in Two-Handed

	//UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	//UBoxComponent* WeaponBox;

	// Sound will be also used for two-handed so it must be protected
	
	//UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	//USoundBase* EquipSound;

	//UPROPERTY(VisibleAnywhere)
	//USceneComponent* BoxTraceStart;

	//UPROPERTY(VisibleAnywhere)
	//USceneComponent* BoxTraceEnd;

// Getter and Setter
public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }

	FORCEINLINE float GetDamage() { return Damage; }

	FORCEINLINE void SetBasicDamage() { Damage = BasicDamage; }

	FORCEINLINE void MultiplyDamage(float Multiplicator) { Damage = BasicDamage * Multiplicator; }

};




