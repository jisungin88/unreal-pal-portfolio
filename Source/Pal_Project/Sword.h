// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Sword.generated.h"

/**
 * 
 */
UCLASS()
class PAL_PROJECT_API ASword : public AWeaponBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword|Hitbox")
	FName BladeStartSocket = TEXT("blade_start");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword|Hitbox")
	FName BladeEndSocket = TEXT("blade_end");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword|Hitbox", meta = (ClampMin = "0"))
	float BladeRadius = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword|Debug")
	bool bShowDebug = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword|Combo")
	TArray<TObjectPtr<UAnimMontage>> ComboMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sword|Combo")
	int32 ComboIndex = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sword|Combo")
	bool bComboWindowOpen = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sword|Combo")
	bool bNextComboQueued = false;

public:
	ASword();

	virtual void Unequip() override;

	virtual void TryAttack() override;

	UFUNCTION(BlueprintCallable, Category = "Sword|Combo")
	void SetComboWindowOpen(bool bOpen);

protected:
	virtual void PerformHitDetection(float DeltaTime) override;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
