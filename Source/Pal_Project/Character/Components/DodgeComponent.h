// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalActionTypes.h"
#include "DodgeComponent.generated.h"

class UAnimMontage;
class APal_ProjectCharacter;
class UPalAnimInstance;

USTRUCT(BlueprintType)
struct FDodgeMontageSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> Forward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> ForwardRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> Right;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> BackwardRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> Backward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> BackwardLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> ForwardLeft;

	UAnimMontage* GetMontage(EDodgeDirection Dir) const;
};

UCLASS( ClassGroup=(Pal), meta=(BlueprintSpawnableComponent) )
class PAL_PROJECT_API UDodgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDodgeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge|Data")
	FDodgeMontageSet DodgeMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Tuning", meta = (ClampMin = "0"))
	float DodgeCooldown = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Tuning")
	EDodgeDirection DefaultDirection = EDodgeDirection::Backward;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge|Tuning", meta = (ClampMin = "0", ClampMax = "1"))
	float InputDeadzone = 0.2f;

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<APal_ProjectCharacter> CachedCharacter;

	UPROPERTY(Transient)
	bool bIsDodging = false;

	float LastDodgeEndTime = -BIG_NUMBER;

	/** AnimInstance에 등록한 델리게이트 */
	FOnMontageEnded MontageEndedDelegate;

public:
	UFUNCTION(BlueprintCallable, Category = "Dodge")
	bool TryDodge();

	UFUNCTION(BlueprintPure, Category = "Dodge")
	bool isDodging() const { return bIsDodging; }

private:
	bool TryCacheOwner();

	EDodgeDirection ResolveDirection() const;

	UFUNCTION()
	void OnDodgeMontageEnded(UAnimMontage* Montage, bool bInterrupted);
		
};
