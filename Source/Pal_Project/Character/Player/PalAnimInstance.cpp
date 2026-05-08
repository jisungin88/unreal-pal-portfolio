// Fill out your co`yright notice in the Description page of Project Settings.

#include "PalAnimInstance.h"
#include "Pal_ProjectCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogPalAnim);

namespace PalAnimDebug
{
    // AddOnScreenDebugMessage 키 — 키가 같으면 메시지가 갱신됨(스팸 방지)
    constexpr int32 LocomotionKey = 1001;
    constexpr int32 StateKey = 1002;
}

bool UPalAnimInstance::TryCacheOwner()
{
    if (OwningCharacter && MovementComponent)
    {
        return true;
    }

    OwningCharacter = Cast<APal_ProjectCharacter>(TryGetPawnOwner());
    if (!OwningCharacter)
    {
        return false;
    }

    MovementComponent = OwningCharacter->GetCharacterMovement();
    if (!MovementComponent)
    {
        UE_LOG(LogPalAnim, Warning, TEXT("OwningCharacter is set but CharacterMovement is null."));
        return false;
    }

    PreviousYaw = OwningCharacter->GetActorRotation().Yaw;
    return true;
}

void UPalAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    TryCacheOwner();
    // 실패해도 OK — Update에서 재시도 (Possess 타이밍 이슈 대비)
}

void UPalAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!TryCacheOwner())
    {
        return;
    }

    // 속도
    const FVector Velocity = OwningCharacter->GetVelocity();
    GroundSpeed = Velocity.Size2D();

    // 가속
    bIsAccelerating = MovementComponent->GetCurrentAcceleration().SizeSquared() > KINDA_SMALL_NUMBER;

    // 체공
    bIsFalling = MovementComponent->IsFalling();
    VerticalVelocity = Velocity.Z;
    TimeInAir = bIsFalling ? (TimeInAir + DeltaSeconds) : 0.f;

    // 이동 방향 (Idle 시에는 0 유지)
    if (GroundSpeed > MovingSpeedThreshold)
    {
        const FRotator ActorRotation = OwningCharacter->GetActorRotation();
        Direction = CalculateDirection(Velocity, ActorRotation);
    }
    else
    {
        Direction = 0.f;
    }

    RotationProfile = OwningCharacter->GetRotationProfile();

    // 캐릭터 회전 (Lean 계산)
    const float CurrentYaw = OwningCharacter->GetActorRotation().Yaw;
    const float YawDelta = FMath::FindDeltaAngleDegrees(PreviousYaw, CurrentYaw);
    YawDeltaSpeed = (DeltaSeconds > KINDA_SMALL_NUMBER)
        ? (YawDelta / DeltaSeconds)
        : 0.f;
    PreviousYaw = CurrentYaw;

    bIsSprinting = OwningCharacter->IsSprinting();

    // Lean: 회전 속도 → 좌우 기울기
    const float TargetLean = FMath::Clamp(YawDeltaSpeed * LeanScale, -1.f, 1.f);
    Lean = FMath::FInterpTo(Lean, TargetLean, DeltaSeconds, LeanInterpSpeed);

    // 상하체 분리 가중치
    // - 이동 속도가 임계값 이상일수록 상체 분리(1.0)
    // - 정지에 가까울수록 전신(0.0)
    // - ForceFullBody 커브가 켜지면 강제로 0으로 끌어당김 (특수 공격 등)
    const float SpeedRatio = GroundSpeed / UpperBodyFullSeparationSpeed;
    float TargetWeight = FMath::Clamp(SpeedRatio, 0.f, 1.f);

    const bool bUpperPlaying = IsAnyMontagePlayingInUpperBodySlot();

    if (bUpperPlaying)
    {
        TargetWeight = 1;
    }
    else
    {
        const float ForceFullBodyValue = GetCurveValue(ForceFullBodyCurveName);
        if (ForceFullBodyValue > KINDA_SMALL_NUMBER)
        {
            TargetWeight = FMath::Lerp(TargetWeight, 0, ForceFullBodyValue);
        }
    }

    UpperBodyBlendWeight = FMath::FInterpTo(UpperBodyBlendWeight, TargetWeight, DeltaSeconds, UpperBodyBlendInterpSpeed);

    // 디버그 출력
    if (bShowAnimDebug && GEngine)
    {
        UAnimMontage* Active = GetCurrentActiveMontage();
        const FString MontageName = Active ? Active->GetName() : TEXT("None");
        const float CurveVal = GetCurveValue(ForceFullBodyCurveName);

        const FString DiagText = FString::Printf(
            TEXT("UpperW=%.2f | Speed=%.0f | ForceFB=%.2f | UpperPlay=%s"),
            UpperBodyBlendWeight,
            GroundSpeed,
            CurveVal,
            IsAnyMontagePlayingInUpperBodySlot() ? TEXT("Y") : TEXT("N"));

        GEngine->AddOnScreenDebugMessage(1004, 0.f, FColor::Orange, DiagText);

        /*const FString BlendText = FString::Printf(
            TEXT("UpperW=%.2f  Speed=%.1f  Moving=%s  Montage=%s"),
            UpperBodyBlendWeight,
            GroundSpeed,
            IsMoving() ? TEXT("YES") : TEXT("no"),
            *MontageName);

        const FString LocoText = FString::Printf(
            TEXT("Speed=%.1f  Dir=%.1f  Yaw/s=%.1f  Lean=%.2f"),
            GroundSpeed, Direction, YawDeltaSpeed, Lean);
        GEngine->AddOnScreenDebugMessage(
            PalAnimDebug::LocomotionKey, 0.f, FColor::Yellow, LocoText);

        const FString StateText = FString::Printf(
            TEXT("Falling=%s  Air=%.2f  VertV=%.1f  UpperW=%.2f  ForceFB=%.2f"),
            bIsFalling ? TEXT("T") : TEXT("F"),
            TimeInAir, VerticalVelocity,
            UpperBodyBlendWeight, ForceFullBodyValue);

        GEngine->AddOnScreenDebugMessage(
            PalAnimDebug::StateKey, 0.f, FColor::Cyan, StateText);*/
    }
}

UAnimMontage* UPalAnimInstance::PlayAttackMontage(
    UAnimMontage* FullBodyMontage, 
    UAnimMontage* UpperBodyMontage,
    bool bForceFullBody,
    float PlayRate)
{
    const bool bUseFullBody = bForceFullBody || !IsMoving();
    UAnimMontage* TargetMontage = bUseFullBody ? FullBodyMontage : UpperBodyMontage;

    if (!IsValid(TargetMontage) && IsValid(FullBodyMontage))
    {
        TargetMontage = FullBodyMontage;
        UE_LOG(LogPalAnim, Verbose, TEXT("UpperBody montage missing — falling back to FullBody"));
    }

    if (!IsValid(TargetMontage))
    {
        UE_LOG(LogPalAnim, Warning, TEXT("PlayAttackMontage failed - both Full/Upper montages are null"));
        return nullptr;
    }

    const float Duration = Montage_Play(TargetMontage, PlayRate);
    if (Duration <= 0)
    {
        UE_LOG(LogPalAnim, Warning, TEXT("Montage_Play returned 0 - '%s' may be invalid"), * TargetMontage->GetName());
        return nullptr;
    }

    UE_LOG(LogPalAnim, Verbose, TEXT("Played '%s' (FullBody=%d, Moving=%d, Force=%d)"),
        *TargetMontage->GetName(),
        bUseFullBody ? 1 : 0, IsMoving() ? 1 : 0, bForceFullBody ? 1 : 0);

    return TargetMontage;
}

bool UPalAnimInstance::IsAnyMontagePlayingInUpperBodySlot() const
{
    UAnimMontage* Active = GetCurrentActiveMontage();
    if (!Active)
    {
        return false;
    }

    for (const FSlotAnimationTrack& Track : Active->SlotAnimTracks)
    {
        if (Track.SlotName == UpperBodySlotName)
        {
            return Montage_IsPlaying(Active);
        }
    }
    return false;
}

void UPalAnimInstance::SetUpperBodyBlendImmediate(float Weight)
{
    UpperBodyBlendWeight = FMath::Clamp(Weight, 0, 1);
}