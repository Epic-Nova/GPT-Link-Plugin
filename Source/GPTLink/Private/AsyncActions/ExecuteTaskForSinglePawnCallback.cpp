/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/
#include "AsyncActions/ExecuteTaskForSinglePawnCallback.h"

UExecuteTaskForSinglePawnCallback::UExecuteTaskForSinglePawnCallback(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, WorldContextObject(nullptr)
{
}

UExecuteTaskForSinglePawnCallback* UExecuteTaskForSinglePawnCallback::ExecuteTaskForSinglePawn(UObject* ContextObject, APawn* Pawn)
{
	UExecuteTaskForSinglePawnCallback* Proxy = NewObject<UExecuteTaskForSinglePawnCallback>();
	Proxy->WorldContextObject = ContextObject;
	Proxy->PawnWeakPtr = Pawn;
	return Proxy;
}

void UExecuteTaskForSinglePawnCallback::Activate()
{
	Super::Activate();

	if(!PawnWeakPtr.Get())
	{
		OnFailure.Broadcast(nullptr, "");
	}

	AGPTLinkManager* GPTManager = AGPTLinkManager::GetGPTLinkManager(WorldContextObject);
	GPTManager->OnTaskExecuted.AddUniqueDynamic(this, &UExecuteTaskForSinglePawnCallback::OnTaskFinished);
	GPTManager->DecideTasks(PawnWeakPtr->GetName());
}

void UExecuteTaskForSinglePawnCallback::SetReadyToDestroy()
{
	Super::SetReadyToDestroy();
	AGPTLinkManager::GetGPTLinkManager(WorldContextObject)->OnTaskExecuted.RemoveDynamic(this, &UExecuteTaskForSinglePawnCallback::OnTaskFinished);
}

void UExecuteTaskForSinglePawnCallback::OnTaskFinished(const APawn* Pawn, const FString TaskSpecificDataString)
{
	NPC = const_cast<APawn*>(Pawn);
	TaskSpecificData = TaskSpecificDataString;
	OnSuccess.Broadcast(Pawn, TaskSpecificDataString);
}
