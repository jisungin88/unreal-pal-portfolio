// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPalCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AMyPalCharacter::AMyPalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")); // 셀카봉 생성
	cameraBoom->SetupAttachment(RootComponent); // 셀카봉을 캐릭터의 Root에 붙임
	cameraBoom->TargetArmLength = 300; // 캐릭터와 카메라의 거리
	cameraBoom->bUsePawnControlRotation = true; // 마우스 회전에 같이 움직이게 함
	cameraBoom->SocketOffset = FVector(0, 50, 50); // 숄더 뷰

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera")); // 카메라 생성
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName); // 카메라를 셀카봉에 붙임
	followCamera->bUsePawnControlRotation = false; // 회전 필요 없음

	// 마우스를 돌릴 때 캐릭터 몸통도 같이 돌지 않게 설정 (카메라만 자유롭게)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터가 이동하는 방향으로 몸통을 자동으로 회전시킴
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // 회전 속도
}

// Called when the game starts or when spawned
void AMyPalCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// 2. 강화된 입력 서브시스템을 가져옵니다.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 3. 여기에 실제 IMC(DefaultMappingContext)를 등록해야 합니다!
			// 이 줄이 없으면 에디터에서 아무리 설정해도 반응이 없습니다.
			Subsystem->AddMappingContext(defaultMappingContext, 0);
		}
	}
}

// Called every frame
void AMyPalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector velocity = GetVelocity(); // 현재 캐릭터의 속도를 가져온다.
	FVector lateralVelocity = FVector(velocity.X, velocity.Y, 0); // 수평 속도만 적용
	groundSpeed = lateralVelocity.Size();

	isFalling = GetCharacterMovement()->IsFalling(); // 공중인지 체크
	//direction = UKismetAnimationLibrary::CalculateDirection(GetVelocity(), GetActorRotation());
	//direction = CalculateDirection(GetVelocity(), GetActorRotation());
}

// Called to bind functionality to input
void AMyPalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComp->BindAction(moveAction, ETriggerEvent::Triggered, this, &AMyPalCharacter::Move);
		enhancedInputComp->BindAction(lookAction, ETriggerEvent::Triggered, this, &AMyPalCharacter::Look);
		enhancedInputComp->BindAction(jumpAction, ETriggerEvent::Started, this, &AMyPalCharacter::Jump);

		enhancedInputComp->BindAction(sprintAction, ETriggerEvent::Started, this, &AMyPalCharacter::StartSprint);
		enhancedInputComp->BindAction(sprintAction, ETriggerEvent::Completed, this, &AMyPalCharacter::StopSprint);
	}
}

void AMyPalCharacter::Move(const FInputActionValue& value)
{
	// 입력 값 가져오기
	FVector2D movementVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 컨트롤러가 바라보는 방향을 기준으로 앞 찾기
		const FRotator rotation = Controller->GetControlRotation();
		const FRotator yawRotation(0, rotation.Yaw, 0);

		// 앞 방향/오른쪽 방향 벡터 계산
		const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(forwardDirection, movementVector.Y); // 상하 이동
		AddMovementInput(rightDirection, movementVector.X); // 좌우 이동
	}
}

void AMyPalCharacter::Look(const FInputActionValue& value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyPalCharacter::StartSprint()
{
	currentState = EPlayerState::Sprinting;
	GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;
}

void AMyPalCharacter::StopSprint()
{
	currentState = EPlayerState::Walking;
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
}

void AMyPalCharacter::StartAiming()
{
	isAiming = true;
	currentState = EPlayerState::Aiming;

	GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;

	GetCharacterMovement()->bOrientRotationToMovement = !isAiming;
	bUseControllerRotationYaw = isAiming;
}

void AMyPalCharacter::StopAiming()
{
	isAiming = false;
	currentState = EPlayerState::Walking;

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	GetCharacterMovement()->bOrientRotationToMovement = !isAiming;
	bUseControllerRotationYaw = isAiming;
}