// Fill out your copyright notice in the Description page of Project Settings.

#include "PalHealthBarWidget.h"
#include "HealthComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPalHealthBarWidget::BindToHealth(UHealthComponent* InHealth)
{
	if (ObservedHealth.IsValid())
	{
		ObservedHealth->OnHealthChanged.RemoveDynamic(this, &UPalHealthBarWidget::HandleHealthChanged);
		ObservedHealth->OnDeath.RemoveDynamic(this, &UPalHealthBarWidget::HandleDeath);
	}

	ObservedHealth = InHealth;

	if (!InHealth) 
	{
		return;
	}

	InHealth->OnHealthChanged.AddDynamic(this, &UPalHealthBarWidget::HandleHealthChanged);
	InHealth->OnDeath.AddDynamic(this, &UPalHealthBarWidget::HandleDeath);

	HandleHealthChanged(InHealth->GetCurrentHealth(), InHealth->GetMaxHealth());
}

void UPalHealthBarWidget::HandleHealthChanged(float NewHealth, float MaxHealth)
{
	if (HealthBar)
	{
		const float Percent = (MaxHealth > KINDA_SMALL_NUMBER) ? (NewHealth / MaxHealth) : 0;
		HealthBar->SetPercent(Percent);
	}

	if (HealthText)
	{
		const FString Text = FString::Printf(TEXT("%.0f / %.0f"), NewHealth, MaxHealth);
		HealthText->SetText(FText::FromString(Text)); // SetText는 FText를 받으므로 FString → FText 변환
	}
}

void UPalHealthBarWidget::HandleDeath()
{
	// 숨김
	SetVisibility(ESlateVisibility::Hidden);
	// Visible		- 보임		레이아웃 공간 차지
	// Hidden		- 안 보임	레이아웃 공간 차지(자리는 유지)
	// Collapsed	- 안 보임	레이아웃 공간 차지 안 함
}

void UPalHealthBarWidget::NativeDestruct()
{
	// 파괴 전 구독 해제
	if (ObservedHealth.IsValid())
	{
		ObservedHealth->OnHealthChanged.RemoveDynamic(this, &UPalHealthBarWidget::HandleHealthChanged);
		ObservedHealth->OnDeath.RemoveDynamic(this, &UPalHealthBarWidget::HandleDeath);
	}

	Super::NativeDestruct();
}