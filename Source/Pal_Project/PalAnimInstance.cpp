// Fill out your copyright notice in the Description page of Project Settings.


#include "PalAnimInstance.h"
#include "Pal_ProjectCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPalAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningCharacter = Cast<APal_ProjectCharacter>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		MovementComponent = OwningCharacter->GetCharacterMovement();
		PreviousYaw = OwningCharacter->GetActorRotation().Yaw;
	}
}

void UPalAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningCharacter || !MovementComponent)
	{
		return;
	}

	// 속도
	const FVector Velocity = OwningCharacter->GetVelocity();
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0).Size();

	// 가속
	bIsAccelerating = MovementComponent->GetCurrentAcceleration().SizeSquared() > 0;

	// 체공
	const bool bWasFalling = bIsFalling;
	bIsFalling = MovementComponent->IsFalling();
	VerticalVelocity = Velocity.Z;

	// 체공 시간
	if (bIsFalling)
	{
		TimeInAir += DeltaSeconds;
	}
	else
	{
		TimeInAir = 0;
	}

	// 이동방향
	if (GroundSpeed > 1)
	{
		const FRotator ActorRotation = OwningCharacter->GetActorRotation();
		Direction = CalculateDirection(Velocity, ActorRotation);
	}
	else
	{
		Direction = 0;
	}

	// 캐릭터 회전
	const float CurrentYaw = OwningCharacter->GetActorRotation().Yaw;
	const float YawDelta = FMath::FindDeltaAngleDegrees(PreviousYaw, CurrentYaw);
	YawDeltaSpeed = (DeltaSeconds > 0) ? (YawDelta / DeltaSeconds) : 0;
	PreviousYaw = CurrentYaw;

	bIsSprinting = OwningCharacter->IsSprinting();

	const float TargetLean = FMath::Clamp(YawDeltaSpeed * LeanScale, -1, 1);
	Lean = FMath::FInterpTo(Lean, TargetLean, DeltaSeconds, LeanInterpSpeed);
}
