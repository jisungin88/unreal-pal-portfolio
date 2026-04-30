#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PalAnimInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPalAnim, Log, All);

UENUM(BlueprintType)
enum class EActionState : uint8
{
    None,
    Attack,
    Dodge,
    Skill
};

class APal_ProjectCharacter;
class UCharacterMovementComponent;

UCLASS()
class PAL_PROJECT_API UPalAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
    /** OwningCharacter/MovementComponent 캐싱. 실패 시 false 반환 */
    bool TryCacheOwner();

    //========================================================
    // Locomotion (런타임 갱신)
    //========================================================
    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Locomotion", meta = (AllowPrivateAccess = "true"))
    float GroundSpeed = 0.f;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Locomotion", meta = (AllowPrivateAccess = "true"))
    float Direction = 0.f;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Locomotion", meta = (AllowPrivateAccess = "true"))
    bool bIsAccelerating = false;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Locomotion", meta = (AllowPrivateAccess = "true"))
    float Lean = 0.f;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Locomotion", meta = (AllowPrivateAccess = "true"))
    float YawDeltaSpeed = 0.f;

    //========================================================
    // State (런타임 갱신)
    //========================================================
    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|State", meta = (AllowPrivateAccess = "true"))
    bool bIsFalling = false;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|State", meta = (AllowPrivateAccess = "true"))
    float VerticalVelocity = 0.f;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|State", meta = (AllowPrivateAccess = "true"))
    float TimeInAir = 0.f;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|State", meta = (AllowPrivateAccess = "true"))
    bool bIsSprinting = false;

    //========================================================
    // Layering (상하체 블렌드)
    //========================================================
    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim|Layering", meta = (AllowPrivateAccess = "true"))
    float UpperBodyBlendWeight = 0.f;

    //========================================================
    // Tuning (디자이너 노출)
    //========================================================
    UPROPERTY(EditDefaultsOnly, Category = "Anim|Tuning", meta = (ClampMin = "0.0"))
    float LeanInterpSpeed = 4.f;

    UPROPERTY(EditDefaultsOnly, Category = "Anim|Tuning", meta = (ClampMin = "0.0"))
    float LeanScale = 0.005f;

    /** 이동 판정 임계값 (cm/s). 이보다 작으면 정지로 간주 */
    UPROPERTY(EditDefaultsOnly, Category = "Anim|Tuning", meta = (ClampMin = "0.0"))
    float MovingSpeedThreshold = 3.f;

    UPROPERTY(EditDefaultsOnly, Category = "Anim|Tuning", meta = (ClampMin = "1.0"))
    float UpperBodyFullSeparationSpeed = 200.f;

    UPROPERTY(EditDefaultsOnly, Category = "Anim|Tuning", meta = (ClampMin = "0.0"))
    float UpperBodyBlendInterpSpeed = 8.f;

    UPROPERTY(EditDefaultsOnly, Category = "Anim|Tuning")
    FName ForceFullBodyCurveName = TEXT("ForceFullBody");

    //========================================================
    // Debug
    //========================================================
    UPROPERTY(EditDefaultsOnly, Category = "Anim|Debug")
    bool bShowAnimDebug = false;

    //========================================================
    // Cached references (소유 액터)
    //========================================================
    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<APal_ProjectCharacter> OwningCharacter;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCharacterMovementComponent> MovementComponent;

    /** 이전 프레임 Yaw — Lean 계산용 (UPROPERTY 불필요) */
    float PreviousYaw = 0.f;

public:
    UFUNCTION(BlueprintCallable, Category = "Pal|Animation")
    UAnimMontage* PlayAttackMontage(UAnimMontage* FullBodyMontage, UAnimMontage* UpperBodyMontage,
        bool bForceFullBody = false, float PlayRate = 1);

    UFUNCTION(BlueprintPure, Category = "Pal|Animation")
    bool IsMoving() const { return GroundSpeed > MovingSpeedThreshold }
};