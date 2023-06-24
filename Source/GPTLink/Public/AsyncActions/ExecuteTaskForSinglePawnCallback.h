/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ExecuteTaskForSinglePawnCallback.generated.h"

UCLASS()
class GPTLINK_API UExecuteTaskForSinglePawnCallback : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UExecuteTaskForSinglePawnCallback(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FExecuableTaskSignature OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FExecuableTaskSignature OnFailure;
	
	UPROPERTY(BlueprintReadOnly, Category = "GPT-Link || Manager || Variables")
	APawn* NPC;

	UPROPERTY(BlueprintReadOnly, Category = "GPT-Link || Manager || Variables")
	FString TaskSpecificData;
	
	UFUNCTION(BlueprintCallable, Category = "GPT-Link || Manager || Functions", meta = (BlueprintInternalUseOnly = "true", WorldContext="ContextObject"))
	static UExecuteTaskForSinglePawnCallback* ExecuteTaskForSinglePawn(UObject* ContextObject, APawn* Pawn);
	
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

	UFUNCTION()
	void OnTaskFinished(const APawn* Pawn, const FString TaskSpecificDataString);	


private:

	TWeakObjectPtr<APawn> PawnWeakPtr;
	UObject* WorldContextObject;
};
