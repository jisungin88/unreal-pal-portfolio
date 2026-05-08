// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingComponent.h"
#include "Buildable.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UBuildingComponent::UBuildingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// ...
}


// Called when the game starts
void UBuildingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (bIsInBuildMode)
	{
		UpdatePreview();
	}
}

void UBuildingComponent::ToggleBuildMode()
{
	bIsInBuildMode = !bIsInBuildMode;
	SetComponentTickEnabled(bIsInBuildMode);

	if (bIsInBuildMode)
	{
		if (!BuildableClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("BuildingComponent: BuildableClass not set."));
			bIsInBuildMode = false;
			return;
		}

		UWorld* World = GetWorld();
		if (!World)
		{
			return;
		}

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Owner = GetOwner();

		PreviewActor = World->SpawnActor<ABuildable>(BuildableClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

		if (PreviewActor)
		{
			PreviewActor->SetPreviewMode(true);
		}
	}
	else
	{
		if (PreviewActor)
		{
			PreviewActor->Destroy();
			PreviewActor = nullptr;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Build mode: %s"), bIsInBuildMode ? TEXT("ON") : TEXT("OFF"));

}

void UBuildingComponent::UpdatePreview()
{
	if (!PreviewActor)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	const FVector TraceStart = CamLoc;
	const FVector TraceEnd = CamLoc + CamRot.Vector() * BuildReach;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PreviewActor);
	QueryParams.AddIgnoredActor(OwnerPawn);

	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	FVector TargetLocation;
	if (bHit)
	{
		
		if (UStaticMeshComponent* PreviewMesh = PreviewActor->GetMesh())
		{
			const FVector BoxExtent = PreviewMesh->Bounds.BoxExtent;
			const float OffsetMagnitude = FMath::Abs(FVector::DotProduct(Hit.ImpactNormal, BoxExtent));
			TargetLocation = Hit.ImpactPoint + Hit.ImpactNormal * OffsetMagnitude;
		}
		else
		{
			TargetLocation = Hit.ImpactPoint;
		}
	}
	else
	{
		TargetLocation = TraceEnd;
	}

	const FVector SnappedLocation = SnapToGrid(TargetLocation);

	const FRotator OwnerRot = OwnerPawn->GetActorRotation();
	const FRotator SnappedRot = FRotator(0, FMath::GridSnap(OwnerRot.Yaw, 90), 0);

	PreviewActor->SetActorLocationAndRotation(SnappedLocation, SnappedRot);

	bIsCurrentlyPlaceable = IsLocationFree(SnappedLocation);
	PreviewActor->SetPlaceable(bIsCurrentlyPlaceable);

	if (bShowDebug)
	{
		DrawDebugLine(World, TraceStart, SnappedLocation, FColor::Cyan, false, 0, 0, 1);
		DrawDebugSphere(World, SnappedLocation, 20, 12, bIsCurrentlyPlaceable ? FColor::Green : FColor::Red, false, 0);
	}
}

FVector UBuildingComponent::SnapToGrid(const FVector& WorldLocation) const
{
	return FVector(
		FMath::GridSnap(WorldLocation.X, GridSize),
		FMath::GridSnap(WorldLocation.Y, GridSize),
		FMath::GridSnap(WorldLocation.Z, GridSize));
}

bool UBuildingComponent::IsLocationFree(const FVector& Location) const
{
	UWorld* World = GetWorld();
	if (!World || !PreviewActor)
	{
		return false;
	}

	UStaticMeshComponent* PreviewMesh = PreviewActor->GetMesh();
	if (!PreviewMesh)
	{
		return false;
	}

	const FVector BoxExtent = PreviewMesh->Bounds.BoxExtent * 0.95f;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PreviewActor);
	QueryParams.AddIgnoredActor(GetOwner());

	const bool bOverlap = World->OverlapAnyTestByChannel(
		Location,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeBox(BoxExtent),
		QueryParams);

	return !bOverlap;
}

bool UBuildingComponent::TryPlaceBuildable()
{
	if (!bIsInBuildMode || !PreviewActor || !bIsCurrentlyPlaceable)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector SpawnLocation = PreviewActor->GetActorLocation();
	const FRotator SpawnRotation = PreviewActor->GetActorRotation();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = GetOwner();

	ABuildable* NewBuildable = World->SpawnActor<ABuildable>(
		BuildableClass, SpawnLocation, SpawnRotation, Params);

	if (!NewBuildable)
	{
		return false;
	}

	NewBuildable->SetPreviewMode(false);

	UE_LOG(LogTemp, Log, TEXT("Placed buildable at %s"), *SpawnLocation.ToString());
	return true;
}