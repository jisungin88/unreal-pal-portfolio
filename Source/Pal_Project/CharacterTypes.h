// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Idle,
	Walking,
	Sprinting,
	Jumping,
	Aiming,
	Attacking,
	CarryingPal,
	Dead,
};