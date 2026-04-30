// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponManager.generated.h"

class AWeaponBase;
class APal_ProjectCharacter;

UCLASS( ClassGroup=(Pal), meta=(BlueprintSpawnableComponent) )
class PAL_PROJECT_API UWeaponManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<TSubclassOf<AWeaponBase>> WeaponLoadout;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AWeaponBase>> SpawnedWeapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentSlot = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AWeaponBase> CurrentWeapon = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EqiupSlot(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	AWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void TryAttack();

private:
	void SpawnWeapons();
};
