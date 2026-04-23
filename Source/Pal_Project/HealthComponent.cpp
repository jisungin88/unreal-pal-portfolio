// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentHealth = MaxHealth;
	bIsDead = false;
}

void UHealthComponent::ApplyDamage(float Amount)
{
	if (Amount <= 0 || bIsDead)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0 && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast();
	}
}

void UHealthComponent::Heal(float Amount)
{
	if (Amount <= 0 || bIsDead) 
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

float UHealthComponent::GetHealthPercent() const
{
	if (MaxHealth <= KINDA_SMALL_NUMBER)
	{
		return 0;
	}

	return CurrentHealth / MaxHealth;
}