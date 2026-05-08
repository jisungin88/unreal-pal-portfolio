// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EActionState : uint8
{
	None,
	Attack,
	Dodge,
	Skill,
};

UENUM(BlueprintType)
enum class EDodgeDirection : uint8
{
	Forward,
	ForwardRight,
	Right,
	BackwardRight,
	Backward,
	BackwardLeft,
	Left,
	ForwardLeft,
};

UENUM(BlueprintType)
enum class ERotationProfile : uint8
{
	Combat,
	Aim,
	Locked, //UMETA(DisplayName = "Locked")
};
