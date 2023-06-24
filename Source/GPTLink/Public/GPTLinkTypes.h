/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/
#pragma once

#include "CoreMinimal.h"
#include "GPTLinkTypes.generated.h"

class UGPTLinkBaseTask;

USTRUCT(BlueprintType)
struct FExecutableTask_S
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GPT-Link")
	bool bIsDefaultTask;
};

USTRUCT(BlueprintType)
struct FGPTRequestNPCData_S
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	FName NPCIdentifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	FString OriginTask;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	FString PreviousTask;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	FVector CurrentPosition;
};

USTRUCT(BlueprintType)
struct FGPTRequestData_S
{
	GENERATED_BODY()

	UPROPERTY()
	FString BeginSeperator =  "BEGIN OF JSON REQUEST";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	TArray<FGPTRequestNPCData_S> NPCData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	TMap<FString, FExecutableTask_S> ExecutableTasks;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	//int32 StepsToPredict;
	
	UPROPERTY()
	FString EndSeperator = "END OF JSON REQUEST";
};

USTRUCT(BlueprintType)
struct FGPTPredictedSteps_S
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GPT-Link")
	FString NextTask;
};

USTRUCT(BlueprintType)
struct FGPTResponseData_S
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GPT-Link")
	FString NPCIdentifier;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GPT-Link")
	FString NextTask;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	int32 TimeToWaitForNextTaskFetch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GPT-Link")
	TArray<FGPTPredictedSteps_S> PredictedSteps;
};

UENUM(BlueprintType)
enum class ETaskStatus : uint8
{
	GPTManagerNotInitialized UMETA(DisplayName = "GPT Manager not Initialized"),
	NotModifiable UMETA(DisplayName = "Not Modifiable"),
	AlreadyRegistered UMETA(DisplayName = "Already Registered"),
	ReadyToInteract UMETA(DisplayName = "Ready to Register"),
};


UENUM(BlueprintType)
enum class ETaskType : uint8
{
	Default,
	Runtime,
	Blueprint,
	None
};

UENUM(BlueprintType)
enum class EGPTModel : uint8
{
	Default,
	Legacy,
	GPT4
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FExecuableTaskSignature, const APawn*, NPC, const FString, TaskSpecificData);