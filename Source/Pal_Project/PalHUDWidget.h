// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalHUDWidget.generated.h"

class UStaminaComponent;

UCLASS()
class PAL_PROJECT_API UPalHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	//T*, TWeakObjectPtr<T>, TObjectPtr<T>
	// 스테미나 컴포넌트 약한 포인터로 보관(TWeakObjectPtr)
	UPROPERTY(Transient) //이 변수는 세이브 파일에 저장하지 않도록 지정
	TWeakObjectPtr<UStaminaComponent> ObservedStamina;

	UPROPERTY(Transient)
	TWeakObjectPtr<class UHealthComponent> ObservedHealth;

public:
	// 어떤 스태미나 컴포넌트를 관찰할지 세팅
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void BindToStamina(UStaminaComponent* InStamina);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void BindToHealth(class UHealthComponent* InHealth);

	// 스태미나 퍼센테이지
	UFUNCTION(BlueprintPure, Category = "HUD")
	float GetStaminaPercent() const;

	UFUNCTION(BlueprintPure, Category = "HUD")
	float GetHealthPercent() const;
};
