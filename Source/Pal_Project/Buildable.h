// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Buildable.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class PAL_PROJECT_API ABuildable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildable();

//protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

//public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	//변수
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buildable|Preview")
	UMaterialInterface* PreviewMaterialOK;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buildable|Preview")
	UMaterialInterface* PreviewMaterialBlocked;

	UPROPERTY(Transient)
	TArray<UMaterialInterface*> OriginalMaterials;

	bool bIsInPreview = false;

	//함수
public:
	// 미리보기
	UFUNCTION(BlueprintCallable, Category = "Buildable")
	void SetPreviewMode(bool bPreview);

	// 배치 가능 여부
	UFUNCTION(BlueprintCallable, Category = "Buildable")
	void SetPlaceable(bool bPlaceable);

	UFUNCTION(BlueprintPure, Category = "Buildable")
	UStaticMeshComponent* GetMesh() const { return MeshComponent; }
};
