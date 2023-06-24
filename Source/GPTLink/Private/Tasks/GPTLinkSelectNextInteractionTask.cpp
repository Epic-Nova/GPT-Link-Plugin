/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/
#include "Tasks/GPTLinkSelectNextInteractionTask.h"
#include "TimerManager.h"

void UGPTLinkSelectNextInteractionTask::ExecuteTask_Internal(const APawn* NPC, FString TaskSpecificData)
{
	Super::ExecuteTask_Internal(NPC, TaskSpecificData);

	TSharedPtr<FJsonValue> JsonValue;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(TaskSpecificData);
	if (FJsonSerializer::Deserialize(Reader, JsonValue))
	{
		if(FString InteractiveTaskData; JsonValue->AsObject()->TryGetStringField("InteractiveTask", InteractiveTaskData))
		{
			UFunction* FunctionToExecute = NPC->FindFunction(*InteractiveTaskData);
			if(!FunctionToExecute)
			{
				UE_LOG(LogGPTLink, Error, TEXT("InteractiveTask %s not found in NPC %s"), *InteractiveTaskData, *NPC->GetName());
				return;
			}
			if(FunctionToExecute->ParmsSize != 0)
			{
				UE_LOG(LogGPTLink, Error, TEXT("InteractiveTask %s has parameters, which is not supported yet, make sure your function has no parameters"), *InteractiveTaskData);
				return;
			}
			APawn* NonConstNPC = const_cast<APawn*>(NPC);
			NonConstNPC->ProcessEvent(FunctionToExecute, nullptr);
		}
	}
	else
	{
		UE_LOG(LogGPTLink, Error, TEXT("Failed to Parse Task information in TaskSpecificData of SelectNextInteractionTask"));
	}
	
}

void UGPTLinkSelectNextInteractionTask::GetTaskSpecificData_Internal(FString& TaskSpecificData)
{
	TArray<TSharedPtr<FJsonObject>> JSONObjects;
	const TSharedRef<FJsonObject> JSONRootObject = MakeShareable(new FJsonObject);
	
	for (FInteractiveTask_S InteractiveTask : InteractiveTasks)
	{
		JSONObjects.Add(FJsonObjectConverter::UStructToJsonObject<FInteractiveTask_S>(InteractiveTask));
	}

	for (const TSharedPtr<FJsonObject> JSONObject : JSONObjects)
	{
		TSharedPtr<FJsonObject> JSONObjectCache = JSONObject;
		
		FString InteractionTaskName	= JSONObjectCache->GetStringField("interactionTaskName");
		JSONObjectCache->RemoveField("interactionTaskName");
		JSONRootObject->SetObjectField(InteractionTaskName, JSONObjectCache);
	}
	
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&TaskSpecificData);
    FJsonSerializer::Serialize(JSONRootObject, Writer);
}
