// Fill out your copyright notice in the Description page of Project Settings.


#include "PalInventoryComponent.h"
#include "PalBase.h"
#include "HealthComponent.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UPalInventoryComponent::UPalInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPalInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Slots.SetNum(MaxSlots);
}

bool UPalInventoryComponent::AddPal(APalBase* PalToStore)
{
	if (!PalToStore)
	{
		return false;
	}

	int32 EmptyIndex = INDEX_NONE;
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (!Slots[i].IsValid())
		{
			EmptyIndex = i;
			break;
		}
	}

	if (EmptyIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory full: cannot store %s"),
			*PalToStore->GetName());
		return false;
	}

	FPalInventoryEntry NewEntry;
	NewEntry.PalClass = PalToStore->GetClass();
	NewEntry.Nickname = PalToStore->GetPalName();
	if (UHealthComponent* Health = PalToStore->GetHealthComponent())
	{
		NewEntry.SavedHealth = Health->GetCurrentHealth();
		NewEntry.SavedMaxHealth = Health->GetMaxHealth();
	}

	Slots[EmptyIndex] = NewEntry;

	UE_LOG(LogTemp, Log, TEXT("Pal stored in slot %d: %s (HP %.0f/%.0f)"),
		EmptyIndex, *NewEntry.Nickname.ToString(),
		NewEntry.SavedHealth, NewEntry.SavedMaxHealth);

	OnInventoryChanged.Broadcast();
	return true;
}

APalBase* UPalInventoryComponent::SummonPal(int32 SlotIndex, const FVector& SummonLocation)
{
	if (!Slots.IsValidIndex(SlotIndex) || !Slots[SlotIndex].IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SummonPal: invalid or empty slot %d"), SlotIndex);
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FPalInventoryEntry& Entry = Slots[SlotIndex];

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APalBase* Summoned = World->SpawnActor<APalBase>(
		Entry.PalClass,
		SummonLocation,
		FRotator::ZeroRotator,
		Params);

	if (!Summoned)
	{
		return nullptr;
	}

	const float HealthToRestore = Entry.SavedHealth;
	World->GetTimerManager().SetTimerForNextTick([Summoned, HealthToRestore]()
		{
			if (!IsValid(Summoned))
			{
				return;
			}

			if (UHealthComponent* Health = Summoned->GetHealthComponent())
			{
				const float Damage = Health->GetMaxHealth() - HealthToRestore;
				if (Damage > 0)
				{
					Health->ApplyDamage(Damage);
				}
			}
		});

	Slots[SlotIndex] = FPalInventoryEntry();

	UE_LOG(LogTemp, Log, TEXT("Pal summoned from slot %d: %s"),
		SlotIndex, *Entry.Nickname.ToString());

	OnInventoryChanged.Broadcast();
	return Summoned;
}

bool UPalInventoryComponent::GetEntry(int32 SlotIndex, FPalInventoryEntry& OutEntry) const
{
	if (!Slots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	OutEntry = Slots[SlotIndex];
	return OutEntry.IsValid();
}

int32 UPalInventoryComponent::GetUsedSlots() const
{
	int32 Count = 0;
	for (const FPalInventoryEntry& Entry : Slots)
	{
		if (Entry.IsValid())
		{
			++Count;
		}
	}

	return Count;
}
