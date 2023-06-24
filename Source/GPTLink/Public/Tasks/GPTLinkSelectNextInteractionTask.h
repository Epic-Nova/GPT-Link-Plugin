/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/

#pragma once

#include "Tasks/GPTLinkBaseTask.h"
#include "GPTLinkSelectNextInteractionTask.generated.h"

USTRUCT(BlueprintType)
struct FInteractiveTaskParameter_S
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	FString ParameterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	FString ParameterValue;
};

USTRUCT(BlueprintType)
struct FInteractiveTask_S
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	FString InteractionTaskName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	TMap<FString, int32> AssociatedTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	TMap<FString, FString> TagDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	TArray<FString> InteractiveTasks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	TMap<FString, FString> ContextualDescriptionInteractiveTasks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	TMap<FString, FString> ContextualInteractiveTasks;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionTaskExecutedSignature, const FInteractiveTask_S&, InteractiveTask, const TArray<FInteractiveTaskParameter_S>&, InteractionTaskParameters);

UCLASS(DisplayName = "Select Next Interaction Task")
class GPTLINK_API UGPTLinkSelectNextInteractionTask : public UGPTLinkBaseTask
{
	GENERATED_BODY()
	
	using FExecutableTask_S = FInteractiveTask_S;
	
public:
	virtual void ExecuteTask_Internal(const APawn* NPC, FString TaskSpecificData) override;
	virtual void GetTaskSpecificData_Internal(FString& TaskSpecificData) override;

	/*UPROPERTY(BlueprintAssignable, Category = "GPT-Link || Select Next Interaction Task || Delegates")
	FInteractionTaskExecutedSignature OnInteractionTaskExecutionPromtReceived;*/ //@TODO
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link || Select Next Interaction Task")
	TArray<FInteractiveTask_S> InteractiveTasks;
};
