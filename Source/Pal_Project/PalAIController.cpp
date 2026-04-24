// Fill out your copyright notice in the Description page of Project Settings.
// Copyright : In / Pal_Project

#include "PalAIController.h"
#include "PalBase.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

APalAIController::APalAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APalAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnerPal = Cast<APalBase>(InPawn);
	if (!OwnerPal)
	{
		UE_LOG(LogTemp, Warning, TEXT("PalAIController: Possessed non-Pal pawn."));
		return;
	}

	if (UPathFollowingComponent* PFC = GetPathFollowingComponent())
	{
		PFC->OnRequestFinished.AddUObject(this, &APalAIController::OnPatrolMoveFinished);
	}

	HomeLocation = OwnerPal->GetActorLocation();
	SetState(EPalAIState::Idle);
}

void APalAIController::OnUnPossess()
{
	OwnerPal = nullptr;
	Super::OnUnPossess();
}

void APalAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!OwnerPal || OwnerPal->GetHealthComponent() == nullptr)
	{
		return;
	}

	switch (CurrentState)
	{
	case EPalAIState::Idle:		TickIdle(DeltaSeconds); break;
	case EPalAIState::Patrol:	TickPatrol(DeltaSeconds); break;
	case EPalAIState::Alert:	TickAlert(DeltaSeconds); break;
	case EPalAIState::Chase:	TickChase(DeltaSeconds); break;
	case EPalAIState::Attack:	TickAttack(DeltaSeconds); break;
	}

	if (bShowDebug && OwnerPal)
	{
		const FString StateText = UEnum::GetValueAsString(CurrentState);
		DrawDebugString(
			GetWorld(),
			OwnerPal->GetActorLocation() + FVector(0, 0, 160),
			StateText,
			nullptr,
			FColor::White,
			0,
			true,
			1.2f);
	}
}

void APalAIController::SetState(EPalAIState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
	switch (NewState)
	{
	case EPalAIState::Idle:
		IdleTimeLeft = IdleDuration;
		StopMovement();
		break;

	case EPalAIState::Patrol:
		bPatrolArrived = false;
		MoveToRandomPointAround(HomeLocation, PatrolRadius);
		break;

	case EPalAIState::Alert:
		StopMovement();
		break;

	case EPalAIState::Chase:
		break;

	case EPalAIState::Attack:
		StopMovement();
		break;
	}
	UE_LOG(LogTemp, Log, TEXT("PalAI: state -> %s"), *UEnum::GetValueAsString(NewState));
}

void APalAIController::TickIdle(float DeltaSeconds)
{
	if (APawn* Player = GetPlayerPawn())
	{
		const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), Player->GetActorLocation());
		if (Dist <= DetectionRange)
		{
			SetState(EPalAIState::Alert);
			return;
		}
	}

	IdleTimeLeft -= DeltaSeconds;
	if (IdleTimeLeft <= 0)
	{
		SetState(EPalAIState::Patrol);
	}
}

void APalAIController::TickPatrol(float DeltaSeconds)
{
	if (APawn* Player = GetPlayerPawn())
	{
		const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), Player->GetActorLocation());
		if (Dist <= DetectionRange)
		{
			SetState(EPalAIState::Alert);
			return;
		}
	}

	if (bPatrolArrived)
	{
		SetState(EPalAIState::Idle);
	}
}

void APalAIController::TickAlert(float DeltaSeconds)
{
	APawn* Player = GetPlayerPawn();
	if (!Player)
	{
		SetState(EPalAIState::Idle);
		return;
	}

	const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), Player->GetActorLocation());
	if (Dist <= ChaseRange)
	{
		SetState(EPalAIState::Chase);
	}
	else if (Dist > DetectionRange)
	{
		SetState(EPalAIState::Idle);
	}
}

void APalAIController::TickChase(float DeltaSeconds)
{
	APawn* Player = GetPlayerPawn();
	if (!Player)
	{
		SetState(EPalAIState::Idle);
		return;
	}

	const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), Player->GetActorLocation());
	if (Dist > LoseTargetRange)
	{
		SetState(EPalAIState::Idle);
		MoveToLocation(HomeLocation);
		return;
	}

	if (Dist <= AttackDistance)
	{
		SetState(EPalAIState::Attack);
		return;
	}

	MoveToActor(Player, 100);
}

void APalAIController::TickAttack(float DeltaSeconds)
{
	APawn* Player = GetPlayerPawn();
	if (!Player)
	{
		SetState(EPalAIState::Idle);
		return;
	}

	const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), Player->GetActorLocation());
	if (Dist > AttackDistance * 1.2f)
	{
		SetState(EPalAIState::Chase);
		return;
	}

	// 플레이어 방향으로 회전 (간단히 — 부드럽게 돌리려면 RInterpTo)
	const FVector ToPlayer = (Player->GetActorLocation() - OwnerPal->GetActorLocation()).GetSafeNormal();
	const FRotator LookRot = ToPlayer.Rotation();
	OwnerPal->SetActorRotation(FRotator(0, LookRot.Yaw, 0));

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime >= AttackCooldown)
	{
		OwnerPal->PerformAttack();
		LastAttackTime = Now;
	}
}

APawn* APalAIController::GetPlayerPawn() const
{
	return UGameplayStatics::GetPlayerPawn(this, 0);
}

void APalAIController::MoveToRandomPointAround(const FVector& Center, float Radius)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		return;
	}

	FNavLocation RandomLocation;
	if (NavSys->GetRandomReachablePointInRadius(Center, Radius, RandomLocation))
	{
		MoveToLocation(RandomLocation.Location);
	}
}

void APalAIController::OnPatrolMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (CurrentState != EPalAIState::Patrol)
	{
		return;
	}

	bPatrolArrived = true;

	UE_LOG(LogTemp, Log, TEXT("Patrol move finished: %s"), *UEnum::GetValueAsString(Result.Code));
}