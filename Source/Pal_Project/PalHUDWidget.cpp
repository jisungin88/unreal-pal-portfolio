// Fill out your copyright notice in the Description page of Project Settings.


#include "PalHUDWidget.h"
#include "StaminaComponent.h"

void UPalHUDWidget::BindToStamina(UStaminaComponent* InStamina)
{
	ObservedStamina = InStamina;
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
