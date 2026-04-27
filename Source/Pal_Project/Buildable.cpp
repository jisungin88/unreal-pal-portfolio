// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildable.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABuildable::ABuildable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

void ABuildable::SetPreviewMode(bool bPreview)
{
	bIsInPreview = bPreview;

	if (!MeshComponent)
	{
		return;
	}

	if (bPreview)
	{
		if (OriginalMaterials.Num() == 0)
		{
			const int32 Num = MeshComponent->GetNumMaterials();
			for (int32 i = 0; i < Num; ++i)
			{
				OriginalMaterials.Add(MeshComponent->GetMaterial(i));
			}
		}

		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetPlaceable(true);
	}
	else
	{
		for (int32 i = 0; i < OriginalMaterials.Num(); ++i)
		{
			MeshComponent->SetMaterial(i, OriginalMaterials[i]);
		}
		MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	}
}

void ABuildable::SetPlaceable(bool bPlaceable)
{
	if (!bIsInPreview || !MeshComponent)
	{
		return;
	}

	UMaterialInterface* TargetMat = bPlaceable ? PreviewMaterialOK : PreviewMaterialBlocked;
	if (!TargetMat)
	{
		return;
	}

	const int32 Num = MeshComponent->GetNumMaterials();
	for (int32 i = 0; i < Num; ++i)
	{
		MeshComponent->SetMaterial(i, TargetMat);
	}
}
