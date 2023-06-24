/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/

/*======================================================================================================================>
| --------------------------------------------------------------------------------------------------------------------- |
|								TO AVOID MISTAKES READ THIS BEFORE EDITING THIS FILE									|
| --------------------------------------------------------------------------------------------------------------------- |
|									USE ONLY FOR INCLUDES THAT ARE FOR THIS MODULE.										|
<======================================================================================================================*/

#pragma once

/*==============================>
| Engine Includes				|
<==============================*/
#include "CoreMinimal.h"
#include "Engine/Engine.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

#include "Kismet/KismetMathLibrary.h"

#include "Subsystems/GameInstanceSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "UObject/UObjectIterator.h"

/*==============================>
| Module Includes				|
<==============================*/
#include "GPTLink.h"
#include "GPTLinkTypes.h"
#include "Tasks/GPTLinkBaseTask.h"