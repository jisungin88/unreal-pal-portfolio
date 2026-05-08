// Fill out your copyright notice in the Description page of Project Settings.


#include "Bow.h"
#include "Pal_ProjectCharacter.h"
#include "PalAnimInstance.h"
#include "PalProjectileBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABow::ABow()
{
	FireMode = ERangedFireMode::Charged;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Category = EWeaponCategory::Ranged;
	WeaponName = TEXT("Bow");

	//물어볼 것 - 부모에서 갖고 있는 데이터이기도하고 blueprint에서 수정할 수 있는거 아닌가?
	BaseProjectileDamage = 30; //세팅할 필요가 있나..?
	FireCooldown = 0.3f; //세팅할 필요가 있나..?
}

void ABow::BeginPlay()
{
}

void ABow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABow::StartFire()
{
	if (!CanFire())
	{
		return;
	}

	if (!bIsAiming)
	{
		UE_LOG(LogTemp, Log, TEXT("[Bow] Cannot fire — not aiming"));
		return;
	}

	// 차징 시작
	bIsCharging = true;
	bIsFiring = true;
	ChargeStartTime = GetWorld()->GetTimeSeconds();
	SetActorTickEnabled(true);

	PlayDrawMontage();

	UE_LOG(LogTemp, Log, TEXT("[Bow] Charge started"));
}

void ABow::StopFire()
{
	if (!bIsCharging)
	{
		Super::StopFire();
		return;
	}

	const float ChargeRatio = GetChargeRatio();
	bIsCharging = false;
	bIsFiring = false;
	SetActorTickEnabled(false);

	if (ChargeRatio < MinChargeToFire)
	{
		// 차징이 부족하면 발사 안 됨 (취소)
		// 시위 당기는 시간 필요
		UE_LOG(LogTemp, Log, TEXT("[Bow] Released too early (%.2f) — canceled"), ChargeRatio);
		return;
	}

	PlayReleaseMontage();
	FireProjectile(ChargeRatio);
}

void ABow::StartAim()
{
	Super::StartAim();
	// 조준 진입 시 별도 모션은 RotationProfile 변경에 따른
	// ABPLayer_Bow 자동 분기로 처리됨
}

void ABow::StopAim()
{

	// 조준 해제 시 차징 중이면 강제 종료 (발사 안 함)
	if (bIsCharging)
	{
		bIsCharging = false;
		SetActorTickEnabled(false);
		UE_LOG(LogTemp, Log, TEXT("[Bow] Charge canceled by aim release"));
	}

	Super::StopAim();
}

void ABow::FireProjectile(float ChargeRatio)
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Bow] ProjectileClass not set"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector SpawnLocation;
	FRotator SpawnRotation;
	CalculateProjectileTransform(SpawnLocation, SpawnRotation);

	FActorSpawnParameters Params;
	Params.Owner = CurrentWielder;
	Params.Instigator = CurrentWielder;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APalProjectileBase* Projectile = World->SpawnActor<APalProjectileBase>(ProjectileClass, SpawnLocation, SpawnRotation, Params);

	if (!Projectile)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Bow] SpawnActor failed"));
		return;
	}

	// 풀차징 보너스
	const float DamageMultiplier = FMath::Lerp(1, FullChargeDamageMultiplier, ChargeRatio);
	const float FinalDamage = BaseProjectileDamage * DamageMultiplier;
	Projectile->SetDamage(FinalDamage);

	if (UProjectileMovementComponent* PMC = Projectile->FindComponentByClass<UProjectileMovementComponent>())
	{
		const float SpeedMultiplier = FMath::Lerp(1, FullChargeSpeedMultiplier, ChargeRatio);
		PMC->InitialSpeed *= SpeedMultiplier;
		PMC->MaxSpeed *= SpeedMultiplier;
		PMC->Velocity = SpawnRotation.Vector() * PMC->InitialSpeed;
	}

	LastFireTime = World->GetTimeSeconds();

	UE_LOG(LogTemp, Log, TEXT("[Bow] Fired (Charge=%.2f, Damage=%.1f)"),	ChargeRatio, FinalDamage);
}

float ABow::GetChargeRatio() const
{
	if (!bIsCharging)
	{
		return 0;
	}

	if (FullChargeTime < KINDA_SMALL_NUMBER)
	{
		return 1;
	}

	const float Elapsed = GetWorld()->GetTimeSeconds() - ChargeStartTime;
	return FMath::Clamp(Elapsed / FullChargeTime, 0, 1);
}

void ABow::PlayDrawMontage()
{
	if (!DrawMontage || !CurrentWielder)
	{
		return;
	}

	USkeletalMeshComponent* Mesh = CurrentWielder->GetMesh();
	UAnimInstance* AnimInst = Mesh ? Mesh->GetAnimInstance() : nullptr;
	if (AnimInst)
	{
		AnimInst->Montage_Play(DrawMontage);
	}
}

void ABow::PlayReleaseMontage()
{
	if (!ReleaseMontage || !CurrentWielder)
	{
		return;
	}

	USkeletalMeshComponent* Mesh = CurrentWielder->GetMesh();
	UAnimInstance* AnimInst = Mesh ? Mesh->GetAnimInstance() : nullptr;
	if (AnimInst)
	{
		AnimInst->Montage_Play(ReleaseMontage);
	}
}
