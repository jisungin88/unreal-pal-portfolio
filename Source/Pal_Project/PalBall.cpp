// Fill out your copyright notice in the Description page of Project Settings.


#include "PalBall.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PalBase.h"
#include "HealthComponent.h"
#include "Engine/DamageEvents.h"
#include "PalAIController.h"

// Sets default values
APalBall::APalBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 충돌체
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->InitSphereRadius(20);
	SphereCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SphereCollision->SetNotifyRigidBodyCollision(true); // Hit 이벤트 수신
	RootComponent = SphereCollision;
	// BlockAll				모든 것과 충돌(벽)
	// BlockAllDynamic		동적 오브젝트(이동체)
	// OverlapAllDynamic	충돌하지만 통과(트리거)
	// NoCollision			충돌 없음(장식)
	
	// 시각용
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetRelativeScale3D(FVector(0.4f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌은 Sphere 담당

	// 투사체 이동
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = SphereCollision;
	ProjectileMovement->InitialSpeed = 0;
	ProjectileMovement->MaxSpeed = 4000;
	ProjectileMovement->ProjectileGravityScale = 1;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.4f;
	ProjectileMovement->Friction = 0.3f;

	InitialLifeSpan = 5;
}

// Called when the game starts or when spawned
void APalBall::BeginPlay()
{
	Super::BeginPlay();
	
	if (SphereCollision)
	{
		SphereCollision->OnComponentHit.AddDynamic(this, &APalBall::OnSphereHit);
	}
}

void APalBall::Launch(const FVector& Direction, float Speed)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = Direction.GetSafeNormal() * Speed;
	}
}

void APalBall::OnSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bHasAttempted || !OtherActor || OtherActor == GetInstigator())
	{
		return;
	}

	if (APalBase* Pal = Cast<APalBase>(OtherActor))
	{
		bHasAttempted = true;

		const bool bSuccess = AttemptCapture(Pal);
		if (bSuccess)
		{
			Pal->Destroy();
			Destroy();
		}
		else
		{
			FDamageEvent DamageEvent;
			Pal->TakeDamage(FailureDamage, DamageEvent, nullptr, this);

			if (APalAIController* AI = Cast<APalAIController>(Pal->GetController()))
			{
				AI->BecomeAggressvie();
			}

			SetLifeSpan(LifetimeAfterHit);
		}
	}
}

bool APalBall::AttemptCapture(APalBase* TargetPal)
{
	if (!TargetPal)
	{
		return false;
	}

	const float Chance = CalculateCaptureChance(TargetPal);
	const float Roll = FMath::FRand(); // 0 ~ 1

	//return Roll <= Chance;

	const bool bSuccess = (Roll <= Chance);
	UE_LOG(LogTemp, Warning,
		TEXT("Capture attempt: Chance=%.2f%%, Roll=%.2f%%, Result=%s"),
		Chance * 100.f, Roll * 100.f,
		bSuccess ? TEXT("SUCCESS") : TEXT("FAILURE"));

	return bSuccess;
}

float APalBall::CalculateCaptureChance(const APalBase* TargetPal) const
{
	if (!TargetPal || !TargetPal->GetHealthComponent())
	{
		return 0;
	}

	const UHealthComponent* Health = TargetPal->GetHealthComponent();
	const float Max = Health->GetMaxHealth();
	if (Max <= KINDA_SMALL_NUMBER)
	{
		return 0;
	}

	const float HealthRatio = Health->GetCurrentHealth() / Max;

	const float Chance = 1.f - FMath::Pow(HealthRatio, CaptureExponent);
	return FMath::Clamp(Chance, 0, 1);
}