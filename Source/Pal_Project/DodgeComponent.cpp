// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeComponent.h"
#include "Pal_ProjectCharacter.h"
#include "PalAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimMontage* FDodgeMontageSet::GetMontage(EDodgeDirection Dir) const
{
	switch (Dir)
	{
	case EDodgeDirection::Forward:       return Forward;
	case EDodgeDirection::ForwardRight:  return ForwardRight;
	case EDodgeDirection::Right:         return Right;
	case EDodgeDirection::BackwardRight: return BackwardRight;
	case EDodgeDirection::Backward:      return Backward;
	case EDodgeDirection::BackwardLeft:  return BackwardLeft;
	case EDodgeDirection::Left:          return Left;
	case EDodgeDirection::ForwardLeft:   return ForwardLeft;
	default:                             return nullptr;
	}
}

UDodgeComponent::UDodgeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDodgeComponent::BeginPlay()
{
	Super::BeginPlay();

	TryCacheOwner();

	MontageEndedDelegate.BindUObject(this, &UDodgeComponent::OnDodgeMontageEnded);
}

bool UDodgeComponent::TryCacheOwner()
{
	if (CachedCharacter.IsValid())
	{
		return true;
	}

	CachedCharacter = Cast<APal_ProjectCharacter>(GetOwner());
	return CachedCharacter.IsValid();
}

bool UDodgeComponent::TryDodge()
{
	if (!TryCacheOwner())
	{
		return false;
	}

	APal_ProjectCharacter* Character = CachedCharacter.Get();

	// 상태 게이팅 — Character가 결정
	if (!Character->CanStartAction(EActionState::Dodge))
	{
		UE_LOG(LogTemp, Verbose, TEXT("[Dodge] Blocked by current state"));
		return false;
	}

	// 쿨다운
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastDodgeEndTime < DodgeCooldown)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[Dodge] Cooldown"));
		return false;
	}

	// 공중에서는 불가
	if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
	{
		if (Move->IsFalling())
		{
			return false;
		}
	}

	// 방향 결정
	const EDodgeDirection Direction = ResolveDirection();
	UAnimMontage* TargetMontage = DodgeMontages.GetMontage(Direction);

	if (!IsValid(TargetMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dodge] Montage missing for direction %s"), *UEnum::GetValueAsString(Direction));
		return false;
	}

	// 재생
	USkeletalMeshComponent* Mesh = Character->GetMesh();
	UAnimInstance* AnimInst = Mesh ? Mesh->GetAnimInstance() : nullptr;
	if (!AnimInst)
	{
		return false;
	}

	const float Duration = AnimInst->Montage_Play(TargetMontage);
	if (Duration <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dodge] Montage_Play failed"));
		return false;
	}

	// 상태 전이 + 종료 콜백
	Character->SetActionState(EActionState::Dodge);
	bIsDodging = true;
	AnimInst->Montage_SetEndDelegate(MontageEndedDelegate, TargetMontage);


	UE_LOG(LogTemp, Log, TEXT("[Dodge] Started %s (Duration=%.2f)"), *UEnum::GetValueAsString(Direction), Duration);

	return true;
}

EDodgeDirection UDodgeComponent::ResolveDirection() const
{
	APal_ProjectCharacter* Character = CachedCharacter.Get();
	if (!Character)
	{
		return DefaultDirection;
	}

	// 마지막 입력 벡터를 캐릭터의 로컬 공간으로 변환
	const FVector WorldInput = Character->GetLastMovementInputVector();
	if (WorldInput.SizeSquared2D() < InputDeadzone * InputDeadzone)
	{
		return DefaultDirection;
	}

	const FRotator ActorRot = Character->GetActorRotation();
	const FVector LocalInput = ActorRot.UnrotateVector(WorldInput);

	// atan2(Y, X) — 정면 기준 시계방향 각도(라디안)
	const float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(LocalInput.Y, LocalInput.X));

	// 8방향으로 분할: 각 섹터는 45도
	const float Normalized = FMath::UnwindDegrees(AngleDeg);
	const int32 Sector = FMath::RoundToInt(Normalized / 45);
	switch (((Sector % 8) + 8) % 8)  // -4 → 4 정규화
	{
	case 0: return EDodgeDirection::Forward;
	case 1: return EDodgeDirection::ForwardRight;
	case 2: return EDodgeDirection::Right;
	case 3: return EDodgeDirection::BackwardRight;
	case 4: return EDodgeDirection::Backward;
	case 5: return EDodgeDirection::BackwardLeft;
	case 6: return EDodgeDirection::Left;
	case 7: return EDodgeDirection::ForwardLeft;
	default: return DefaultDirection;
	}
}

void UDodgeComponent::OnDodgeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsDodging = false;
	LastDodgeEndTime = GetWorld()->GetTimeSeconds();

	if (APal_ProjectCharacter* Character = CachedCharacter.Get())
	{
		Character->SetActionState(EActionState::None);
	}

	UE_LOG(LogTemp, Verbose, TEXT("[Dodge] Ended (Interrupted=%d)"), bInterrupted ? 1 : 0);
}