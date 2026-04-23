// Fill out your copyright notice in the Description page of Project Settings.


#include "PalBase.h"
#include "HealthComponent.h"
#include "StaminaComponent.h"
#include "PalHealthBarWidget.h"
#include "Components/WidgetComponent.h"

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

	// 디버그 타이머 (Day 6에서 넣은 그대로)
	if (HealthComponent)
	{
		FTimerHandle DebugTimer;
		GetWorldTimerManager().SetTimer(DebugTimer, [this]()
			{
				if (HealthComponent)
				{
					HealthComponent->ApplyDamage(30.f);
				}
			}, 3.f, true);   // ← false를 true로 바꿔서 3초마다 반복 (확인용)
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