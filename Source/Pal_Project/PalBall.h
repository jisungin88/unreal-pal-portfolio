// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PalBall.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class APalBase;

UCLASS()
class PAL_PROJECT_API APalBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APalBall();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// 언리얼이 제공하는 포물선 운동 자동 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	// 낮을수록 포획 쉬움
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Capture", meta = (ClampMin = "0.1"))
	float CaptureExponent = 1.5f;

	// 포획 시도 후 구체가 사라지기까지의 지연 (실패 시 튕기는 시간 확보) 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Capture", meta = (ClampMin = "0.0"))
	float LifetimeAfterHit = 2.f;

	// 포획 실패 시 팰에게 가할 미량의 데미지 (상처 입히는 효과)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Capture", meta = (ClampMin = "0.0"))
	float FailureDamage = 5.f;

	// 이미 포획 판정을 한 번 했는지 (중복 방지)
	bool bHasAttempted = false;

protected:
	UFUNCTION()
	void OnSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 포획 시도: 확률 계산 후 성공/실패 처리. 반환값 = 성공 여부
	bool AttemptCapture(APalBase* TargetPal);

	// 포획 성공률 계산
	float CalculateCaptureChance(const APalBase* TargetPal) const;

	bool IsFriendlyPal(const APalBase* Pal) const;

public:
	// 발사 방향 설정 (Spawn 직후 호출)
	UFUNCTION(BlueprintCallable, Category = "PalBall")
	void Launch(const FVector& Direction, float Speed);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

//public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
