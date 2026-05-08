// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedWeaponBase.h"
#include "Pal_ProjectCharacter.h"
#include "PalProjectileBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"

ARangedWeaponBase::ARangedWeaponBase()
{
	Category = EWeaponCategory::Ranged;
}

void ARangedWeaponBase::StartAim()
{
	if (!CurrentWielder || bIsAiming)
	{
		return;
	}

	bIsAiming = true;
	CurrentWielder->SetRotationProfile(ERotationProfile::Aim);

	UE_LOG(LogTemp, Log, TEXT("[Ranged] StartAim — %s"), *GetName());
}

void ARangedWeaponBase::StopAim()
{
	if (!CurrentWielder || !bIsAiming)
	{
		return;
	}

	bIsAiming = false;
	CurrentWielder->SetRotationProfile(ERotationProfile::Combat);

	if (bIsFiring)
	{
		StopFire();
	}

	UE_LOG(LogTemp, Log, TEXT("[Ranged] StopAim"));
}

void ARangedWeaponBase::StartFire()
{
	if (!CanFire())
	{
		return;
	}

	bIsFiring = true;

	if (FireMode == ERangedFireMode::Instant)
	{
		FireProjectile(1);
		bIsFiring = false;
	}
}

void ARangedWeaponBase::StopFire()
{
	bIsFiring = false;
}

bool ARangedWeaponBase::CanFire() const
{
	if (!CurrentWielder)
	{
		return false;
	}

	if (GetWorld()->GetTimeSeconds() - LastFireTime < FireCooldown)
	{
		return false;
	}

	return true;
}

void ARangedWeaponBase::FireProjectile(float ChargeRatio)
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Ranged] ProjectileClass not set — %s"), *GetName());
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

	APalProjectileBase* Projectile = World->SpawnActor<APalProjectileBase>(
		ProjectileClass, SpawnLocation, SpawnRotation, Params);

	if (!Projectile)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Ranged] SpawnActor failed"));
		return;
	}

	const float Damage = BaseProjectileDamage * ChargeRatio;

	LastFireTime = World->GetTimeSeconds();
	UE_LOG(LogTemp, Log, TEXT("[Ranged] Fired (Charge=%.2f, Damage=%.1f)"), ChargeRatio, Damage);
}

void ARangedWeaponBase::CalculateProjectileTransform(FVector& OutLocation, FRotator& OutRotation) const
{
	// Projectile 스폰 위치 - 무기 메시의 muzzle 소켓
	if (WeaponMesh && WeaponMesh->DoesSocketExist(MuzzleSocketName))
	{
		OutLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
	}
	else
	{
		OutLocation = WeaponMesh ? WeaponMesh->GetComponentLocation() : GetActorLocation();
	}

	// 발사 방향
	if (CurrentWielder)
	{
		if (UCameraComponent* Camera = CurrentWielder->GetFollowCamera())
		{
			// 카메라 위치에서 정면으로 LineTrace해서 조준점 찾기
			// 조준점을 향하는 방향 계산
			const FVector CameraLocation = Camera->GetComponentLocation();
			const FVector CameraForward = Camera->GetForwardVector();
			const FVector AimTarget = CameraLocation + CameraForward * MaxRange;

			const FVector ToTarget = (AimTarget - OutLocation).GetSafeNormal();
			OutRotation = ToTarget.Rotation();
			return;
		}
	}

	OutRotation = WeaponMesh ? WeaponMesh->GetComponentRotation() : GetActorRotation();
}