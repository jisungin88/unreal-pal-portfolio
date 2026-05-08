// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangedWeaponBase.h"
#include "Bow.generated.h"

/**
 * 
 */
UCLASS()
class PAL_PROJECT_API ABow : public ARangedWeaponBase
{
	GENERATED_BODY()
	
// =====================================================
// Constructor
// =====================================================
public:
	ABow();

// =====================================================
// Engine Overrides
// =====================================================
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

// =====================================================
// Runtime State
// =====================================================
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bow|State")
	bool bIsCharging = false;

	/** 차징 시작 시각 */
	float ChargeStartTime = 0.f;

// =====================================================
// Animation
// =====================================================
protected:
	// 차징 시작 시 재생할 몽타주 (시위 당기기)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Animation")
	TObjectPtr<UAnimMontage> DrawMontage;

	// 발사 시 재생할 몽타주 (시위 놓기)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Animation")
	TObjectPtr<UAnimMontage> ReleaseMontage;

// =====================================================
// Combat
// =====================================================
protected:
	// 차징 풀까지 걸리는 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Charge",	meta = (ClampMin = "0.1"))
	float FullChargeTime = 1.2f;

	// 발사 가능 최소 차징 비율 (이보다 적으면 발사 안 됨)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Charge",	meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinChargeToFire = 0.2f;

	// 풀차징 보너스 데미지 배율 (1.0=기본, 2.0=2배)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Charge",	meta = (ClampMin = "1.0"))
	float FullChargeDamageMultiplier = 2.0f;

	// 풀차징 시 Projectile 속도 배율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow|Charge",	meta = (ClampMin = "1.0"))
	float FullChargeSpeedMultiplier = 1.5f;

// =====================================================
// Weapon Overrides
// =====================================================
public:
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void StartAim() override;
	virtual void StopAim() override;

protected:
	// 풀차징 비율로 데미지/속도 보정해서 발사
	virtual void FireProjectile(float ChargeRatio) override;

// =====================================================
// Public Functions
// =====================================================
public:
	UFUNCTION(BlueprintPure, Category = "Bow|Charge")
	float GetChargeRatio() const;

	UFUNCTION(BlueprintPure, Category = "Bow|Charge")
	bool IsCharging() const { return bIsCharging; }

// =====================================================
// Internal Functions
// =====================================================
protected:
	// 상태에 따른 몽타주 재생
	void PlayDrawMontage();
	void PlayReleaseMontage();
};
