// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class USkeletalMeshComponent;
class APal_ProjectCharacter;
class UAnimMontage;

UENUM(BlueprintType)
enum class EWeaponCategory : uint8
{
	None,
	Melee,
	Ranged,
};

UCLASS(Abstract, Blueprintable)
class PAL_PROJECT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	EWeaponCategory Category = EWeaponCategory::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Data")
	FName WeaponName = TEXT("Unnamed Weapon");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Attachment")
	FName AttachSocketName = TEXT("hand_r_socket");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0"))
	float AttackCooldown = 0.6f;

	float LastAttackTime = 0;

	UPROPERTY(Transient)
	TObjectPtr<APal_ProjectCharacter> CurrentWielder;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> HitActorsThisSwing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Combat")
	bool bHitDetectionActive = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0"))
	float BaseDamage = 25;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void EqiupTo(APal_ProjectCharacter* Wielder);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Unequip();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void TryAttack();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponCategory GetCategory() const { return Category; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	APal_ProjectCharacter* GetWielder() const { return CurrentWielder; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool IsEquipped() const { return CurrentWielder != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
	virtual void SetHitDetectionEnabled(bool bEnabled);

protected:
	virtual void Tick(float DeltaTime) override;

	virtual void PerformHitDetection(float DeltaTime);
};
