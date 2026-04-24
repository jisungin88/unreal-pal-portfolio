// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalInventoryComponent.generated.h"

class APalBase;

USTRUCT(BlueprintType)
struct FPalInventoryEntry
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APalBase> PalClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SavedHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SavedMaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Nickname;

	bool IsValid() const { return PalClass != nullptr; }
};

// 델리게이트: 인벤토리 변경 시 브로드캐스트 (HUD 갱신용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAL_PROJECT_API UPalInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPalInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal|Inventory")
	TArray<FPalInventoryEntry> Slots;
	// Slots.SetNum(MaxSlots);  // 크기 고정
	// Slots.IsValidIndex(idx); // 범위 체크
	// Slots.Num();             // 크기

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pal|Inventory", meta = (ClampMin = "1"))
	int32 MaxSlots = 6;
		
public:
	UFUNCTION(BlueprintCallable, Category = "Pal|Inventory")
	bool AddPal(APalBase* PalToStore);

	UFUNCTION(BlueprintCallable, Category = "Pal|Inventory")
	APalBase* SummonPal(int32 SlotIndex, const FVector& SummonLocation);

	UFUNCTION(BlueprintPure, Category = "Pal|Inventory")
	bool GetEntry(int32 SlotIndex, FPalInventoryEntry& OutEntry) const;

	UFUNCTION(BlueprintPure, Category = "Pal|Inventory")
	int32 GetMaxSlots() const { return MaxSlots; }

	UFUNCTION(BlueprintPure, Category = "Pal|Inventory")
	int32 GetUsedSlots() const;

	// 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Pal|Inventory")
	FOnInventoryChanged OnInventoryChanged;
};
