/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/
#include "Tasks/GPTLinkMoveToLocationTask.h"

void UGPTLinkMoveToLocationTask::ExecuteTask_Internal(const APawn* NPC, FString TaskSpecificData)
{
	Super::ExecuteTask_Internal(NPC, TaskSpecificData);
	
	TSharedPtr<FJsonValue> JsonValue;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(TaskSpecificData);
	if (FJsonSerializer::Deserialize(Reader, JsonValue))
	{
		FVector Location;
		if(FString LocationData; JsonValue->AsObject()->TryGetStringField("Location", LocationData) && Location.InitFromString(LocationData))
		{
			OnLocationSelected.Broadcast(NPC, Location);
		}
	}
	else
	{
		UE_LOG(LogGPTLink, Error, TEXT("Failed to Parse Location information in TaskSpecificData of MoveToLocationTask"));
	}
}

void UGPTLinkMoveToLocationTask::GetTaskSpecificData_Internal(FString& TaskSpecificData)
{
	TArray<AActor*> GPTLinkActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), GPTLinkActors);

	const TSharedPtr<FJsonObject> JSONObject = MakeShareable(new FJsonObject);
	const TSharedRef<FJsonObject> JSONRootObject = MakeShareable(new FJsonObject);
	
	for (const AActor* Element : GPTLinkActors)
	{
		if(!Element) continue;
		if(!Element->Tags.Contains("NPCInteractable")) continue;
			
		JSONObject->SetStringField(Element->GetHumanReadableName(), Element->GetActorLocation().ToString());
	}
	
	JSONRootObject->SetObjectField("AvailableLocations", JSONObject);
	
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&TaskSpecificData);
	FJsonSerializer::Serialize(JSONRootObject, Writer);
}
