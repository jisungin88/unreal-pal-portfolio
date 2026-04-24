// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "PalAIController.generated.h"

class APalBase;

// enum class (C++11) 네임스페이스 분리.
// uint8 - BlueprintType으로 노출하려면 필수
UENUM(BlueprintType)
enum class EPalAIState : uint8 
{
	Idle,	// UMETA(DisplayName = "Idle")
	Patrol,
	Alert,
	Chase,
	Attack,
};

UCLASS()
class PAL_PROJECT_API APalAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	APalAIController();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Perception")
	float DetectionRange = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Perception")
	float ChaseRange = 500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Perception")
	float LoseTargetRange = 1500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Patrol")
	float PatrolRadius = 800;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Patrol")
	float IdleDuration = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Combat", meta = (Clamp = "0"))
	float AttackDistance = 180;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Combat", meta = (Clamp = "0.1"))
	float AttackCooldown = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Debug")
	bool bShowDebug = true;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
	EPalAIState CurrentState = EPalAIState::Idle;

	FVector HomeLocation = FVector::ZeroVector;
	float IdleTimeLeft = 0;
	bool bPatrolArrived = false;
	float LastAttackTime = 0;

	UPROPERTY(Transient)
	APalBase* OwnerPal = nullptr;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaSeconds) override;

	// 세팅
	void SetState(EPalAIState NewState);

	// 이동 로직
	void TickIdle(float DeltaSeconds);
	void TickPatrol(float DeltaSeconds);
	void TickAlert(float DeltaSeconds);
	void TickChase(float DeltaSeconds);
	void TickAttack(float DeltaSeconds);

	class APawn* GetPlayerPawn() const;

	void MoveToRandomPointAround(const FVector& Center, float Radius);

	void OnPatrolMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result);

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	void BecomeAggressvie();
};
