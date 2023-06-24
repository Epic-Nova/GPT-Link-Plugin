/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/
#pragma once

#include "Tasks/GPTLinkBaseTask.h"
#include "GPTLinkCustomTask.generated.h"

UCLASS(DisplayName = "Custom Task")
class GPTLINK_API UGPTLinkCustomTask : public UGPTLinkBaseTask
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link || Custom Task")
	TMap<FName, FString> JSONObject;
	virtual void ExecuteTask_Internal(const APawn* NPC, FString TaskSpecificData) override;
};
