// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PalBase.generated.h"

class UHealthComponent;
class UStaminaComponent;
class UWidgetComponent;

UCLASS()
class PAL_PROJECT_API APalBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APalBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	// virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = "Pal")
	UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UFUNCTION(BlueprintPure, Category = "Pal")
	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }

	UFUNCTION(BlueprintPure, Category = "Pal")
	FName GetPalName() const { return PalDisplayName; }

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent, // 데미지 타입/위치 정보
		class AController* EventInstigator,	// 보상·처치 기여도
		AActor* DamageCauser) override;			// 데미지를 실제로 발생시킨 물체 (투사체, 무기 등)

	UFUNCTION(BlueprintCallable, Category = "Pal|Combat")
	bool PerformAttack();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Stats")
	UStaminaComponent* StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|UI")
	UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pal|Info")
	FName PalDisplayName = TEXT("Unnamed Pal");
	// FName		- 고정된 식별자(크고 빠른 비교, 변경 거의 안 됨)
	// FString	- 수정 가능한 일반 문자열
	// FText		- UI에 표시되는 현지화 가능 문자열

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pal|Combat", meta = (ClampMin = "0"))
	float AttackDamage = 15;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pal|Combat", meta = (ClampMin = "0"))
	float AttackRange = 180;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pal|Combat", meta = (ClampMin = "0"))
	float AttackRadius = 80;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pal|Combat|Debug")
	bool bShowAttackDebug = true;

	UFUNCTION() // 델리게이트 바인딩용
	void HandleDeath();

};
