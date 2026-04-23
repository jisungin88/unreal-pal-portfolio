// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "InputActionValue.h" // 입력을 받기 위한 도구
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MyPalCharacter.generated.h"

UCLASS()
class PAL_PROJECT_API AMyPalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyPalCharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* cameraBoom; // 셀카봉

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* followCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EPlayerState currentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool isCarryingPal;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool isAiming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* defaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* moveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* lookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* jumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* sprintAction;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AimingSpeed = 300;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float walkSpeed = 400;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float sprintSpeed = 800;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float groundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool isFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float direction;

	float NormalWalkSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& value);

	void Look(const FInputActionValue& value);

	void StartSprint();

	void StopSprint();

	void StartAiming();

	void StopAiming();

public:	
	void SetCharacterState(EPlayerState newState);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
