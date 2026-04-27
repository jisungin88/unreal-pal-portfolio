// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h" //언리얼이 쓰는 기본 타입(FString, FVector, int32 등) 꾸러미
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Pal_ProjectCharacter.generated.h" //무조건 인클루드 맨 마지막 줄. 언리얼 빌드툴(UHT)이 자동 생성하는 리플렉션 코드. 위치 바뀌면 빌드 깨집니다.

//헤더에서는 포인터/참조로만 쓸 거라면 전방 선언으로 충분
//헤더는 얇게, 실제 #include는 .cpp에서. 나중에 프로젝트 커질수록 이게 체감
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class APalBall;
class UStaminaComponent;
class UHealthComponent;
class UPalInventoryComponent;
class UBuildingComponent;
class UPalHUDWidget;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game) // 언리얼 리플렉션 시스템에 등록
class APal_ProjectCharacter : public ACharacter
{
	GENERATED_BODY() // UHT가 자동 생성한 리플렉션 보일러플레이트 코드가 들어간다는 표식

	//VisibleAnywhere		- 에디터에서 보이긴 하는데 수정 불가 (컴포넌트처럼 "존재" 자체만 확인)
	//EditAnywhere			- 에디터에서 값을 자유롭게 수정 가능
	//VisibleDefaultsOnly	- 블루프린트 에셋에선 수정 가능, 레벨에 배치된 인스턴스는 수정 불가
	//EditDefaultsOnly		- 블루프린트 에셋에선 수정 가능, 레벨에 배치된 인스턴스는 수정 불가
	//BlueprintReadOnly		- 블루프린트에서 읽기만 가능
	//BlueprintReadWrite	- 블루프린트에서 읽기 / 쓰기 둘 다
	//Category = "..."		- 에디터 디테일 창에서 어느 섹션에 묶어 보일지
	//meta = (AllowPrivateAccess = "true") - private인데도 블루프린트에서 접근 허용
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SummonAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BuildModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PlaceAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UStaminaComponent* StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UPalInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UBuildingComponent* BuildingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float WalkSpeed = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float SprintSpeed = 800;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackDamage = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackRange = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackRadius = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Debug", meta = (AllowPrivateAccess = "true"))
	bool bShowAttackDebug = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwing", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APalBall> PalBallClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing", meta = (AllowPrivateAccess = "true"))
	float ThrowSpeed = 2500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing", meta = (AllowPrivateAccess = "true"))
	FVector ThrowOffset = FVector(80, 30, 40);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPalHUDWidget> HUDWidgetClass; //클래스 자체(설계도)를 가리킴

	UPROPERTY(Transient)
	UPalHUDWidget* HUDWidget; //인스턴스(실제 위젯 객체)를 가리킴

public:
	APal_ProjectCharacter();
	
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
	void StartSprint(const FInputActionValue& Value);

	void StopSprint(const FInputActionValue& Value);

	void ForceStopSprint();

	void Attack(const FInputActionValue& Value);

	void Throw(const FInputActionValue& Value);

	void SummonFirstPal(const FInputActionValue& Value);

	void OnBuildModeToggle(const FInputActionValue& Value);

	void OnPlaceBuildable(const FInputActionValue& Value);

	void CreateHUDWidget();
	
	UFUNCTION()
	void HandleDeath();

public:
	UFUNCTION(BlueprintPure, Category = "Pal")
	UPalInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	//FORCEINLINE = "함수 호출 비용 없이 호출한 자리에 코드를 직접 박아넣어라" (성능 최적화)
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

