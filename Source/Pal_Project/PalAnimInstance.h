// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PalAnimInstance.generated.h"

class APal_ProjectCharacter;
class UCharacterMovementComponent;

UCLASS()
class PAL_PROJECT_API UPalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	// Transient - 세이브 파일에 저장하지 않는다.
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Locomotion", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed = 0;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Locomotion", meta = (AllowPrivateAccess = "true"))
	float Direction = 0;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Locomotion", meta = (AllowPrivateAccess = "true"))
	float Lean = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim|Tuning", meta = (AllowPrivateAccess = "true"))
	float LeanInterpSpeed = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim|Tuning", meta = (AllowPrivateAccess = "true"))
	float LeanScale = 0.005f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|State", meta = (AllowPrivateAccess = "true"))
	bool bIsFalling = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|State", meta = (AllowPrivateAccess = "true"))
	float VerticalVelocity = 0;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|State", meta = (AllowPrivateAccess = "true"))
	float TimeInAir = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim|Tuning", meta = (AllowPrivateAccess = "true"))
	float JumpToFallThreshold = 1;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|State", meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Locomotion", meta = (AllowPrivateAccess = "true"))
	float YawDeltaSpeed = 0;

private:
	float PreviousYaw = 0;

protected:
	virtual void NativeInitializeAnimation() override; //BeginPlay 직후 1회

	virtual void NativeUpdateAnimation(float DeltaSeconds) override; // 매 프레임

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APal_ProjectCharacter> OwningCharacter;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
	
};
