// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingComponent.generated.h"

class ABuildable;

UCLASS( ClassGroup=(Pal), meta=(BlueprintSpawnableComponent) )
class PAL_PROJECT_API UBuildingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuildingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	TSubclassOf<ABuildable> BuildableClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building", meta = (ClampMin = "1"))
	float GridSize = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building", meta = (ClampMin = "0"))
	float BuildReach = 800;

	UPROPERTY(EditDefaultsOnly, BlueprintREadOnly, Category = "Building|Debug")
	bool bShowDebug = true;

	UPROPERTY(Transient)
	ABuildable* PreviewActor = nullptr;

	bool bIsInBuildMode = false;
	bool bIsCurrentlyPlaceable = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Building")
	void ToggleBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Building")
	bool TryPlaceBuildable();

	UFUNCTION(BlueprintPure, Category = "Building")
	bool IsInBuildMode() const { return bIsInBuildMode; }

protected:
	void UpdatePreview();

	FVector SnapToGrid(const FVector& WorldLocation) const;

	bool IsLocationFree(const FVector& Location) const;
		
};
