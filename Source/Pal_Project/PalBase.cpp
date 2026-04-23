// Fill out your copyright notice in the Description page of Project Settings.


#include "PalBase.h"
#include "HealthComponent.h"
#include "StaminaComponent.h"
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