// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_HitWindow.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Pal_ProjectCharacter.h"
#include "WeaponManager.h"
#include "WeaponBase.h"

void UAnimNotifyState_HitWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
			float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}

	APal_ProjectCharacter* Character = Cast<APal_ProjectCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	UWeaponManager* WM = Character->GetWeaponManager();
	if (!WM)
	{
		return;
	}

	AWeaponBase* Weapon = WM->GetCurrentWeapon();
	if (Weapon)
	{
		Weapon->SetHitDetectionEnabled(true);
	}
}

void UAnimNotifyState_HitWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	APal_ProjectCharacter* Character = Cast<APal_ProjectCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		return;
	}

	UWeaponManager* WM = Character->GetWeaponManager();
	if (!WM)
	{
		return;
	}
	
	AWeaponBase* Weapon = WM->GetCurrentWeapon();
	if (Weapon)
	{
		Weapon->SetHitDetectionEnabled(false);
	}
}
