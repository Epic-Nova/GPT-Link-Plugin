/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/
#include "Components/NPCDefinition.h"


UNPCDefinition::UNPCDefinition()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNPCDefinition::BeginPlay()
{
	Super::BeginPlay();
	
}

void UNPCDefinition::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

