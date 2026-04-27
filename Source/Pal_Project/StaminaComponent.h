// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

//ClassGroup=(Custom) - 에디터 "Add Component" 드롭다운에서 "Pal"이라는 카테고리로 묶임
//BlueprintSpawnableComponent: 블루프린트 에디터에서 "Add Component" 버튼으로 추가 가능하게 해주는 핵심 메타
UCLASS( ClassGroup=(Pal), meta=(BlueprintSpawnableComponent) )
class PAL_PROJECT_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStaminaComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "1.0"))
	float MaxStamina = 100; //최대 스태미나

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "0.0"))
	float RegenRate = 20; //스태미나 초당 재생량

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "0"))
	float DrainRate = 25; //스태미나 초당 소모량

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float CurrentStamina = 100; //현재 스태미나

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
 	bool bIsDraining = false; //소모중인지 여부

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//BlueprintPure		- 읽기만 (상태를 변경 안 함). const 함수랑 궁합.	초록색, 실행 핀 없음 (getter 용)
	//BlueprintCallable	- 변경 가능 (부르면 상태 바뀜)					파란색, 실행 핀 있음
	//스태미나를 소모해도 되는지의 여부
	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool HasEnoughStamina(float Amount) const { return CurrentStamina >= Amount; };

	//Amount만큼 스태미나 소모
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ConsumeStamina(float Amount);

	//스태미나량 조회
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetCurrentStamina() const { return CurrentStamina; }

	//최대 스태미나량 조회
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetMaxStamina() const { return MaxStamina; }

	//지속 소모 상태 토글
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetDraining(bool bNewDraining) { bIsDraining = bNewDraining; }
		
};
