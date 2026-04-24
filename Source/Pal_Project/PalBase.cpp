// Fill out your copyright notice in the Description page of Project Settings.


#include "PalBase.h"
#include "HealthComponent.h"
#include "StaminaComponent.h"
#include "PalHealthBarWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "DrawDebugHelpers.h"

// Sets default values
APalBase::APalBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));

	HealthBarWidget->SetupAttachment(GetRootComponent());
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen); // 항상 카메라를 바라봄 (Billboard)
	HealthBarWidget->SetRelativeLocation(FVector(0, 0, 120)); // 머리 위
	HealthBarWidget->SetDrawSize(FVector2D(150, 20)); // 사이즈
}

// Called when the game starts or when spawned
void APalBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &APalBase::HandleDeath);
	}

	// WidgetComponent의 UserWidget 인스턴스를 가져와서 타입 캐스팅
	if (HealthBarWidget && HealthComponent)
	{
		FTimerHandle BindTimer;
		GetWorldTimerManager().SetTimerForNextTick([this]()
			{
				if (!HealthBarWidget || !HealthComponent)
				{
					return;
				}

				UUserWidget* UserWidget = HealthBarWidget->GetUserWidgetObject();
				if (UPalHealthBarWidget* HealthBar = Cast<UPalHealthBarWidget>(UserWidget))
				{
					HealthBar->BindToHealth(HealthComponent);
				}
			});
	}
}

void APalBase::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetPalName().ToString());

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	// 충돌 비활성화
	SetActorEnableCollision(false);

	// 5초 후 소멸
	SetLifeSpan(5);
}

float APalBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0 || !HealthComponent || HealthComponent->IsDead())
	{
		return 0;
	}

	HealthComponent->ApplyDamage(ActualDamage);


	UE_LOG(LogTemp, Log, TEXT("%s took %.1f damage from %s"),
		*GetPalName().ToString(),
		ActualDamage,
		DamageCauser ? *DamageCauser->GetName() : TEXT("Unknown"));

	return ActualDamage;
}

bool APalBase::PerformAttack()
{
	UWorld* World = GetWorld();
	if (!World || !HealthComponent || HealthComponent->IsDead())
	{
		return false;
	}

	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<FHitResult> HitResults;
	const bool bHit = World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AttackRadius),
		QueryParams);

	if (bShowAttackDebug)
	{
		DrawDebugLine(World, Start, End, FColor::Orange, false, 0.5f, 0, 2);
		DrawDebugSphere(World, End, AttackRadius, 12, bHit ? FColor::Red : FColor::Green, false, 0.5f);
	}

	if (!bHit)
	{
		return false;
	}

	bool bDealtDamage = false;
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == this)
		{
			continue;
		}

		if (HitActor->IsA(APalBase::StaticClass()))
		{
			continue;
		}

		FDamageEvent DamageEvent;
		HitActor->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
		bDealtDamage = true;
	}
	return bDealtDamage;
}