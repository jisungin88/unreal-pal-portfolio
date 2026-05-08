// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_ComboWindow.h"
#include "Pal_ProjectCharacter.h"
#include "WeaponManager.h"
#include "Sword.h"

void UAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UE_LOG(LogTemp, Warning, TEXT("### ComboWindow NotifyBegin called!"));   // ← 추가

	if (!MeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("  → MeshComp is null"));
		return;
	}

	APal_ProjectCharacter* Character = Cast<APal_ProjectCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("  → Character cast failed"));
		return;
	}

	UWeaponManager* WM = Character->GetWeaponManager();
	if (!WM)
	{
		UE_LOG(LogTemp, Warning, TEXT("  → WeaponManager null"));
		return;
	}

	if (ASword* Sword = Cast<ASword>(WM->GetCurrentWeapon()))
	{
		Sword->SetComboWindowOpen(true);
		UE_LOG(LogTemp, Warning, TEXT("  → Sword ComboWindow OPEN ✓"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  → Current weapon is not Sword (or null)"));
	}
}

void UAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UE_LOG(LogTemp, Warning, TEXT("### ComboWindow NotifyEnd called!"));   // ← 추가

	if (!MeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("  → MeshComp is null"));
		return;
	}

	APal_ProjectCharacter* Character = Cast<APal_ProjectCharacter>(MeshComp->GetOwner());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("  → Character cast failed"));
		return;
	}

	UWeaponManager* WM = Character->GetWeaponManager();
	if (!WM)
	{
		UE_LOG(LogTemp, Warning, TEXT("  → WeaponManager null"));
		return;
	}

	if (ASword* Sword = Cast<ASword>(WM->GetCurrentWeapon()))
	{
		Sword->SetComboWindowOpen(false);
		UE_LOG(LogTemp, Warning, TEXT("  → Sword ComboWindow Close ✓"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  → Current weapon is not Sword (or null)"));
	}
}