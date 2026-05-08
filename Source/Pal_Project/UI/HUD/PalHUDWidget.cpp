// Fill out your copyright notice in the Description page of Project Settings.


#include "PalHUDWidget.h"
#include "StaminaComponent.h"
#include "HealthComponent.h"

void UPalHUDWidget::BindToStamina(UStaminaComponent* InStamina)
{
	ObservedStamina = InStamina;
}

void UPalHUDWidget::BindToHealth(UHealthComponent* InHealth)
{
	ObservedHealth = InHealth;
}

float UPalHUDWidget::GetStaminaPercent() const
{
	if (!ObservedStamina.IsValid())
		return 0.f;

	const float Max = ObservedStamina->GetMaxStamina();
	if (Max <= KINDA_SMALL_NUMBER) // 1e-4. 언리얼 제공 상수. 0과 "0에 가까운 아주 작은 값"을 구분할 때 씀
		return 0.f;

	return ObservedStamina->GetCurrentStamina() / Max;
}

float UPalHUDWidget::GetHealthPercent() const
{
	if (!ObservedHealth.IsValid())
	{
		return 0;
	}

	const float Max = ObservedHealth->GetMaxHealth();
	if (Max <= KINDA_SMALL_NUMBER)
	{
		return 0;
	}

	return ObservedHealth->GetCurrentHealth() / Max;
}
