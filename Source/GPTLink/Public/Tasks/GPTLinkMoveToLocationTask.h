/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/

#pragma once

#include "Tasks/GPTLinkBaseTask.h"
#include "GPTLinkMoveToLocationTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLocationSelectedSignature, const APawn*, NPC, const FVector&, Location);	

UCLASS(DisplayName = "Move To Location Task")
class GPTLINK_API UGPTLinkMoveToLocationTask : public UGPTLinkBaseTask
{
	GENERATED_BODY()

public:
	virtual void ExecuteTask_Internal(const APawn* NPC, FString TaskSpecificData) override;
	virtual void GetTaskSpecificData_Internal(FString& TaskSpecificData) override;


	UPROPERTY(BlueprintAssignable, Category = "GPT-Link || Move To Location Task || Delegates")
	FOnLocationSelectedSignature OnLocationSelected;
};

