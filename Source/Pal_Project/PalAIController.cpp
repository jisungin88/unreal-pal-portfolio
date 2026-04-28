// Fill out your copyright notice in the Description page of Project Settings.
// Copyright : In / Pal_Project

#include "PalAIController.h"
#include "PalBase.h"
#include "HealthComponent.h"
#include "Engine/OverlapResult.h"
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
	case EPalAIState::Follow:	TickFollow(DeltaSeconds); break;
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

		if (CurrentTarget.IsValid())
		{
			DrawDebugLine(GetWorld(),
				OwnerPal->GetActorLocation(),
				CurrentTarget->GetActorLocation(),
				FColor::Red, false, 0.f, 0, 2.f);
		}
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

	case EPalAIState::Follow:
		break;
	}
	UE_LOG(LogTemp, Log, TEXT("PalAI: state -> %s"), *UEnum::GetValueAsString(NewState));
}

void APalAIController::TickIdle(float DeltaSeconds)
{
	if (bIsCompanion)
	{
		SetState(EPalAIState::Follow);
		return;
	}

	if (APawn* Player = GetPlayerPawn())
	{
		const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), Player->GetActorLocation());
		if (Dist <= DetectionRange)
		{
			CurrentTarget = Player;
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
	if (bIsCompanion)
	{
		SetState(EPalAIState::Follow);
		return;
	}

	if (APawn* Player = GetPlayerPawn())
	{
		const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), Player->GetActorLocation());
		if (Dist <= DetectionRange)
		{
			CurrentTarget = Player;
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
	if (!CurrentTarget.IsValid())
	{
		SetState(EPalAIState::Idle);
		return;
	}

	const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (Dist <= ChaseRange)
	{
		SetState(EPalAIState::Chase);
	}
	else if (Dist > DetectionRange)
	{
		CurrentTarget.Reset();
		SetState(EPalAIState::Idle);
	}
}

void APalAIController::TickChase(float DeltaSeconds)
{
	if (!IsTargetValid(LoseTargetRange))
	{
		CurrentTarget.Reset();

		if (bIsCompanion)
		{
			SetState(EPalAIState::Follow);
		}
		else
		{
			SetState(EPalAIState::Idle);
			MoveToLocation(HomeLocation);
		}
		return;
	}

	AActor* Target = CurrentTarget.Get();
	const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), Target->GetActorLocation());
	if (Dist <= AttackDistance)
	{
		SetState(EPalAIState::Attack);
		return;
	}

	MoveToActor(Target, 100);
}

void APalAIController::TickAttack(float DeltaSeconds)
{
	if (!IsTargetValid(AttackDistance * 2))
	{
		CurrentTarget.Reset();
		if (bIsCompanion)
		{
			SetState(EPalAIState::Follow);
		}
		else
		{
			SetState(EPalAIState::Idle);
		}
		return;
	}

	AActor* Target = CurrentTarget.Get();
	const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), Target->GetActorLocation());
	if (Dist > AttackDistance * 1.2f)
	{
		SetState(EPalAIState::Chase);
		return;
	}

	// 플레이어 방향으로 회전 (간단히 — 부드럽게 돌리려면 RInterpTo)
	const FVector ToTarget = (Target->GetActorLocation() - OwnerPal->GetActorLocation()).GetSafeNormal();
	OwnerPal->SetActorRotation(FRotator(0, ToTarget.Rotation().Yaw, 0));

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime >= AttackCooldown)
	{
		OwnerPal->PerformAttack();
		LastAttackTime = Now;
	}
}

void APalAIController::TickFollow(float DeltaSeconds)
{
	APawn* Player = GetPlayerPawn();
	if (!Player)
	{
		StopMovement();
		return;
	}

	if (APalBase* Enemy = FindNearestEnemyPal(CompanionDetectRadius))
	{
		CurrentTarget = Enemy;
		SetState(EPalAIState::Chase);
		return;
	}

	const float Dist = FVector::Dist(OwnerPal->GetActorLocation(), Player->GetActorLocation());

	const float StopDist = 250;
	const float FollowDist = 400;

	if (Dist > FollowDist)
	{
		MoveToActor(Player, StopDist);
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

void APalAIController::BecomeAggressive()
{
	if (CurrentState != EPalAIState::Chase && CurrentState != EPalAIState::Attack)
	{
		SetState(EPalAIState::Chase);
	}
}

void APalAIController::SetCompanionMode(bool bCompanion)
{
	bIsCompanion = bCompanion;

	if (bCompanion)
	{
		SetState(EPalAIState::Follow);
	}
	else
	{
		SetState(EPalAIState::Idle);
	}
}

APalBase* APalAIController::FindNearestEnemyPal(float SearchRadius) const
{
	if (!OwnerPal)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerPal);

	const FVector Center = OwnerPal->GetActorLocation();
	const bool bAny = World->OverlapMultiByChannel(
		Overlaps,
		Center,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SearchRadius),
		QueryParams);

	if (!bAny)
	{
		return nullptr;
	}

	APalBase* Nearest = nullptr;
	float NearestDistSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Overlap : Overlaps)
	{
		APalBase* Candidate = Cast<APalBase>(Overlap.GetActor());
		if (!Candidate || Candidate == OwnerPal)
		{
			continue;
		}

		// 이미 죽은 팰은 무시
		if (UHealthComponent* Health = Candidate->GetHealthComponent())
		{
			if (Health->IsDead())
			{
				continue;
			}
		}

		// 같은 컴패니언끼리는 적 아님
		if (APalAIController* OtherAI = Cast<APalAIController>(Candidate->GetController()))
		{
			if (OtherAI->IsCompanion())
			{
				continue;
			}

			if (!bIsCompanion)
			{
				continue;
			}
		}

		// 거리 비교 (sqrt 안 쓰는 SizeSquared 최적화)
		const float DistSq = FVector::DistSquared(Center, Candidate->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			Nearest = Candidate;
		}
	}
	return Nearest;
}

bool APalAIController::IsTargetValid(float MaxDistance) const
{
	if (!CurrentTarget.IsValid() || !OwnerPal)
	{
		return false;
	}

	AActor* Target = CurrentTarget.Get();

	if (APalBase* TargetPal = Cast<APalBase>(Target))
	{
		if (TargetPal->GetHealthComponent() && TargetPal->GetHealthComponent()->IsDead())
		{
			return false;
		}
	}

	const float DistSq = FVector::DistSquared(OwnerPal->GetActorLocation(), Target->GetActorLocation());
	return DistSq <= (MaxDistance * MaxDistance);
}