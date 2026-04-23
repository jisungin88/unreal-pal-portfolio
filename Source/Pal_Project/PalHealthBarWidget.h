// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalHealthBarWidget.generated.h"

class UHealthComponent;
class UProgressBar;
class UTextBlock;

UCLASS()
class PAL_PROJECT_API UPalHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeDestruct() override;

public :
	UFUNCTION(BlueprintCallable, Category = "Pal|UI")
	void BindToHealth(UHealthComponent* InHealth);

protected:
	UFUNCTION() // 델리게이트 콜백
	void HandleHealthChanged(float NewHealth, float MaxHealth);

	UFUNCTION()
	void HandleDeath();

protected:
	// HealthBar라는 이름의 ProgressBar가 있으면, 컴파일 시 C++ 변수에 자동으로 연결
	// *주의 이름 불일치 = 컴파일 실패.
	UPROPERTY(meta = (BindWidget)) 
	UProgressBar* HealthBar;

	// 없어도 OK
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* HealthText;

	UPROPERTY(Transient)
	TWeakObjectPtr<UHealthComponent> ObservedHealth;
};
