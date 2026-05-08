// Fill out your copyright notice in the Description page of Project Settings.


#include "PalProjectileBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/DamageEvents.h"

// Sets default values
APalProjectileBase::APalProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// 콜리전 세팅
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(SphereSize);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionComp->OnComponentHit.AddDynamic(this, &APalProjectileBase::OnHit);
	RootComponent = CollisionComp;

	// 메시 세팅
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(CollisionComp);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 이동 컴포넌트
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = bShouldBounce;
	ProjectileMovement->ProjectileGravityScale = GravityScale;
}

// Called when the game starts or when spawned
void APalProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	// 자동 파괴
	SetLifeSpan(LifeSpan);
}

void APalProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Hit %s (Damage=%.1f)"),	*OtherActor->GetName(), Damage);

	// 데미지 적용
	FDamageEvent DamageEvent;
	AController* OwnerController = GetInstigatorController();
	OtherActor->TakeDamage(Damage, DamageEvent, OwnerController, this);

	// 첫 충돌 후 사라짐 (또는 박힘)
	Destroy();
}
