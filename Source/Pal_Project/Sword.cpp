// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword.h"
#include "Pal_ProjectCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "PalAnimInstance.h"
#include "Engine/DamageEvents.h" 

ASword::ASword()
{
	Category = EWeaponCategory::Melee;
	WeaponName = TEXT("Sword");
	BaseDamage = 25;
	AttackCooldown = 0.1f;
}

void ASword::PerformHitDetection(float DeltaTime)
{
	if (!WeaponMesh || !CurrentWielder)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Start = WeaponMesh->GetSocketLocation(BladeStartSocket);
	const FVector End = WeaponMesh->GetSocketLocation(BladeEndSocket);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(CurrentWielder);

	TArray<FHitResult> Hits;
	const bool bAny = World->SweepMultiByChannel(
		Hits, Start, End, FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(BladeRadius),
		Params);

	if (bShowDebug)
	{
		const FColor LineColor = bAny ? FColor::Red : FColor::Green;
		DrawDebugLine(World, Start, End, LineColor, false, 0.1f, 0, 1.5f);
	}

	if (!bAny)
	{
		return;
	}

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == CurrentWielder)
		{
			continue;
		}

		if (HitActorsThisSwing.Contains(HitActor))
		{
			continue;
		}

		HitActorsThisSwing.Add(HitActor);

		FDamageEvent DamageEvent;
		HitActor->TakeDamage(BaseDamage, DamageEvent, CurrentWielder->GetController(), this);

		UE_LOG(LogTemp, Log, TEXT("Sword hit: %s (%.1f damage)"), *HitActor->GetName(), BaseDamage);
	}
}

void ASword::TryAttack()
{
	if (!CurrentWielder || ComboMontages.Num() == 0)
	{
		return;
	}

	USkeletalMeshComponent* CharacterMesh = CurrentWielder->GetMesh();
	if (!CharacterMesh)
	{
		return;
	}

	UPalAnimInstance* PalAnim = Cast<UPalAnimInstance>(CharacterMesh->GetAnimInstance());
	if (!PalAnim)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sword: AnimInstance is not UPalAnimInstance"));
		return;
	}

	if (ComboIndex >= 0)
	{
		if (!bComboWindowOpen || ComboIndex + 1 >= ComboMontages.Num())
		{
			return;
		}

		const FAttackMontagePair& NextPair = ComboMontages[++ComboIndex];
		UAnimMontage* Played = PalAnim->PlayAttackMontage(
			NextPair.FullBodyMontage,
			NextPair.UpperBodyMontage,
			NextPair.bForceFullBody);

		if (!Played)
		{
			ComboIndex = -1;
			return;
		}

		UE_LOG(LogTemp, Warning,
			TEXT("Sword: Play start - Montage='%s' Duration=%.2f"),
			*Played->GetName(), Played->GetPlayLength());

		FOnMontageEnded EndedDelegate;
		EndedDelegate.BindUObject(this, &ASword::OnAttackMontageEnded);
		PalAnim->Montage_SetEndDelegate(EndedDelegate, Played);

		bComboWindowOpen = false;
		UE_LOG(LogTemp, Log, TEXT("Sword: combo continued -> %d"), ComboIndex + 1);
		return;
	}

	const  FAttackMontagePair& FirstPair = ComboMontages[0];
	UAnimMontage* Played = PalAnim->PlayAttackMontage(
		FirstPair.FullBodyMontage,
		FirstPair.UpperBodyMontage,
		FirstPair.bForceFullBody);

	if (!Played)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sword: failed to start combo"));
		return;
	}


	if (Played == FirstPair.UpperBodyMontage)
	{
		PalAnim->SetUpperBodyBlendImmediate(1);
	}

	ComboIndex = 0;

	FOnMontageEnded EndedDelegate;
	EndedDelegate.BindUObject(this, &ASword::OnAttackMontageEnded);
	PalAnim->Montage_SetEndDelegate(EndedDelegate, Played);

	UE_LOG(LogTemp, Log, TEXT("Sword: combo started (1)"));
}

void ASword::SetComboWindowOpen(bool bOpen)
{
	bComboWindowOpen = bOpen;
}

void ASword::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const FString Reason = bInterrupted ? TEXT("INTERRUPTED") : TEXT("FINISHED");
	UE_LOG(LogTemp, Warning,
		TEXT("Sword: '%s' ended (%s) ComboIdx=%d"),
		Montage ? *Montage->GetName() : TEXT("null"),
		*Reason, ComboIndex);

	if (bInterrupted)
	{
		UE_LOG(LogTemp, Log, TEXT("Sword: montage interrupted (combo continues)"));
		return;
	}

	// 자연 종료 = 콤보 끝까지 다 갔거나 클릭 안 함
	ComboIndex = -1;
	bComboWindowOpen = false;
	bNextComboQueued = false;
	UE_LOG(LogTemp, Log, TEXT("Sword: combo ended/reset"));
}

void ASword::Unequip()
{
	Super::Unequip();
	ComboIndex = -1;
	bComboWindowOpen = false;
	bNextComboQueued = false;
}