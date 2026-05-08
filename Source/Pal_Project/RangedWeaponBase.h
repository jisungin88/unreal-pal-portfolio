// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "RangedWeaponBase.generated.h"

class AProjectileBase;
class APalProjectileBase;

UENUM(BlueprintType)
enum class ERangedFireMode : uint8
{
	Instant,
	Charged,
};

UCLASS(Abstract, Blueprintable)
class PAL_PROJECT_API ARangedWeaponBase : public AWeaponBase
{
	GENERATED_BODY()
	
// Property
public:

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged|Combat")
	ERangedFireMode FireMode = ERangedFireMode::Charged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged|Combat")
	TSubclassOf<APalProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged|Combat")
	FName MuzzleSocketName = TEXT("muzzle"); 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged|Combat", meta = (ClampMin = "50"))
	float MaxRange = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged|Combat", meta = (ClampMin = "0"))
	float BaseProjectileDamage = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged|Combat", meta = (ClampMin = "0"))
	float FireCooldown = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged|Combat")
	bool bIsAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged|Combat")
	bool bIsFiring = false;

	float LastFireTime = -BIG_NUMBER;

private:

// Function
public:
	ARangedWeaponBase();

	UFUNCTION(BlueprintCallable, Category = "Ranged")
	virtual void StartAim();

	UFUNCTION(BlueprintCallable, Category = "Ranged")
	virtual void StopAim();

	UFUNCTION(BlueprintCallable, Category = "Ranged")
	virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Ranged")
	virtual void StopFire();
	
	UFUNCTION(BlueprintPure, Category = "Ranged")
	bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintPure, Category = "Ranged")
	bool IsFiring() const { return bIsFiring; }

protected:
	virtual void FireProjectile(float ChargeRatio);

	virtual bool CanFire() const;

	virtual void CalculateProjectileTransform(FVector& OutLocation, FRotator& OutRotator) const;

private:
};
