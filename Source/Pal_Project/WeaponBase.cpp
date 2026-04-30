// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Pal_ProjectCharacter.h"
#include "Animation/AnimInstance.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = WeaponMesh;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponBase::EqiupTo(APal_ProjectCharacter* Wielder)
{
	if (!Wielder)
	{
		return;
	}

	CurrentWielder = Wielder;

	USkeletalMeshComponent* CharacterMesh = Wielder->GetMesh();
	if (CharacterMesh)
	{
		const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		AttachToComponent(CharacterMesh, Rules, AttachSocketName);
	}

	SetActorHiddenInGame(false);
}

void AWeaponBase::Unequip()
{
	CurrentWielder = nullptr;

	const FDetachmentTransformRules Rules(EDetachmentRule::KeepRelative, true);
	DetachFromActor(Rules);

	SetActorHiddenInGame(true);
}

void AWeaponBase::TryAttack()
{
	if (!CurrentWielder)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < AttackCooldown)
	{
		return;
	}

	LastAttackTime = Now;

	if (AttackMontage)
	{
		USkeletalMeshComponent* CharacterMesh = CurrentWielder->GetMesh();
		if (CharacterMesh)
		{
			if (UAnimInstance* AnimInst = CharacterMesh->GetAnimInstance())
			{
				AnimInst->Montage_Play(AttackMontage);
			}
		}
	}
}

void AWeaponBase::SetHitDetectionEnabled(bool bEnabled)
{
	bHitDetectionActive = bEnabled;
	SetActorTickEnabled(bEnabled);

	if (bEnabled)
	{
		HitActorsThisSwing.Reset();
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHitDetectionActive)
	{
		PerformHitDetection(DeltaTime);
	}
}

void AWeaponBase::PerformHitDetection(float DeltaTime)
{

}