// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManager.h"
#include "WeaponBase.h"
#include "Pal_ProjectCharacter.h"
#include "Engine/World.h"

UWeaponManager::UWeaponManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnWeapons();
}

void UWeaponManager::SpawnWeapons()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APal_ProjectCharacter* Owner = Cast<APal_ProjectCharacter>(GetOwner());
	if (!Owner)
	{
		return;
	}
	
	for (TSubclassOf<AWeaponBase> WeaponClass : WeaponLoadout)
	{
		if (!WeaponClass)
		{
			continue;
		}

		FActorSpawnParameters Params;
		Params.Owner = Owner;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AWeaponBase* NewWeapon = World->SpawnActor<AWeaponBase>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
		if (NewWeapon)
		{
			NewWeapon->SetActorHiddenInGame(true);
			SpawnedWeapons.Add(NewWeapon);
		}
	}
}

void UWeaponManager::EqiupSlot(int32 SlotIndex)
{
	if (SlotIndex == CurrentSlot)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->Unequip();
		}
		CurrentWeapon = nullptr;
		CurrentSlot = -1;
		UE_LOG(LogTemp, Log, TEXT("Weapon uneqiupped."));
		return;
	}

	if (!SpawnedWeapons.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid weapon slot: %d"), SlotIndex);
		return;
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->Unequip();
	}

	APal_ProjectCharacter* Owner = Cast<APal_ProjectCharacter>(GetOwner());
	if (!Owner)
	{
		return;
	}

	CurrentWeapon = SpawnedWeapons[SlotIndex];
	CurrentSlot = SlotIndex;

	if (CurrentWeapon)
	{
		CurrentWeapon->EqiupTo(Owner);
		UE_LOG(LogTemp, Log, TEXT("Equipped weapon at slot %d"), SlotIndex);
	}
}

void UWeaponManager::TryAttack()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->TryAttack();
	}
}