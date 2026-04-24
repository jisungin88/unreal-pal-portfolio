// Copyright Epic Games, Inc. All Rights Reserved.

#include "Pal_ProjectCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "StaminaComponent.h"
#include "HealthComponent.h"
#include "PalHUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "PalBase.h"                          
#include "Kismet/GameplayStatics.h"           // ApplyDamage
#include "Engine/DamageEvents.h"              // FDamageEvent
#include "DrawDebugHelpers.h"                 // 디버그 라인

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APal_ProjectCharacter

APal_ProjectCharacter::APal_ProjectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;					// 점프 초기 속도 (cm/s)
	GetCharacterMovement()->AirControl = 0.35f;						// 공중 제어도 (0=못움직임, 1=땅처럼 자유)
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;				// 최대 걷기 속도 (cm/s)
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;				// 아날로그 스틱 살짝 기울였을 때 최소속도
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;	// 땅에서 멈출 때 감속
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;	// 공중에서 멈출 때 감속

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	// 스테미나
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	// 체력
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void APal_ProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	CreateHUDWidget();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &APal_ProjectCharacter::HandleDeath);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APal_ProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APal_ProjectCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APal_ProjectCharacter::Look);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APal_ProjectCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APal_ProjectCharacter::StopSprint);

		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APal_ProjectCharacter::Attack);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APal_ProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APal_ProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APal_ProjectCharacter::StartSprint(const FInputActionValue& Value)
{
	if (StaminaComponent && !StaminaComponent->HasEnoughStamina(1.f))
		return;

	bIsSprinting = true;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement()) 
	{
		Movement->MaxWalkSpeed = SprintSpeed;
	}

	if (StaminaComponent)
	{
		StaminaComponent->SetDraining(true);
	}
}

void APal_ProjectCharacter::StopSprint(const FInputActionValue& Value)
{
	ForceStopSprint();
}

void APal_ProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 대시 중인데 스태미나 바닥이면 강제 중단
	if (bIsSprinting && StaminaComponent && StaminaComponent->GetCurrentStamina() <= 0.f)
	{
		ForceStopSprint();
	}
}

void APal_ProjectCharacter::ForceStopSprint()
{
	bIsSprinting = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = WalkSpeed;
	}

	if (StaminaComponent)
	{
		StaminaComponent->SetDraining(false);
	}
}

void APal_ProjectCharacter::CreateHUDWidget()
{
	//서버 전용 인스턴스 등에서는 UI 생성 스킵
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC || !PC->IsLocalController())
		return;

	if (!HUDWidgetClass)
	{
		//UE_LOG(LogTemplateCharacter, Warning, TEXT("HUDWidgetClass가 지정되지 않았습니다. 블루프린트에서 설정하세요."));
		return;
	}

	HUDWidget = CreateWidget<UPalHUDWidget>(PC, HUDWidgetClass); // 언리얼 UI 생성 표준 함수
	if (HUDWidget)
	{
		HUDWidget->BindToStamina(StaminaComponent);
		HUDWidget->BindToHealth(HealthComponent);
		HUDWidget->AddToViewport();
	}
}

void APal_ProjectCharacter::Attack(const FInputActionValue& Value)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Start = GetActorLocation();
	const FVector Forward = GetActorForwardVector();
	const FVector End = Start + Forward * AttackRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<FHitResult> HitResults;
	// Sweep = 이동하며 스캔, Overlap = 특정 위치에 겹친 것만
	// Multi = 여러 개, Single = 첫 하나만
	const bool bHit = World->SweepMultiByChannel(
		HitResults,                                    // [출력] 맞은 것들 배열
		Start,                                         // 시작점
		End,                                           // 끝점
		FQuat::Identity,                               // 도형 회전 (Sphere는 회전 의미 없음)
		ECC_Pawn,                                      // 충돌 채널 (Pawn 채널은 캐릭터들을 잡음. ECC_WorldStatic은 벽/지형)
		FCollisionShape::MakeSphere(AttackRadius),     // 도형 정의
		QueryParams);                                  // 옵션
	
	if (bShowAttackDebug)
	{
		DrawDebugLine(World, Start, End, FColor::Yellow, false, 1, 0, 2);
		DrawDebugSphere(World, End, AttackRadius, 12, bHit ? FColor::Red : FColor::Green, false, 1);
	}

	if (!bHit)
	{
		return;
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == this)
		{
			continue;
		}

		if (APalBase* Pal = Cast<APalBase>(HitActor))
		{
			FDamageEvent DamageEvent;
			Pal->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
		}
	}
}

float APal_ProjectCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0 || !HealthComponent || HealthComponent->IsDead())
	{
		return 0;
	}

	HealthComponent->ApplyDamage(ActualDamage);
	UE_LOG(LogTemp, Log, TEXT("Player took %.1f damage from %s. HP: %.1f"),
		ActualDamage,
		DamageCauser ? *DamageCauser->GetName() : TEXT("Unknown"),
		HealthComponent->GetCurrentHealth());

	return ActualDamage;
}

void APal_ProjectCharacter::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Player has died."));

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->RemoveMappingContext(DefaultMappingContext);
			}
		}
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->DisableMovement();
	}
}
