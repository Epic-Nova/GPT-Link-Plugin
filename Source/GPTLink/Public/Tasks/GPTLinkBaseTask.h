/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/
#pragma once

#include "GPTLinkTypes.h"
#include "UObject/Object.h"
#include "JsonObjectConverter.h"
#include "GPTLinkManager.h"
#include "GPTLinkBaseTask.generated.h"

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, DisplayName = "GPT-Link Task")
class GPTLINK_API UGPTLinkBaseTask : public UObject
{
	GENERATED_BODY()
	
public:
	
	/**
	 * Executes the Task, normally this function should be called by the GPT Manager and not by the user
	 *
	 * @param NPC The NPC that should execute the task
	 * @param TaskSpecificData Task specific data that is needed to execute the task (MUST BE IN JSON FORMAT)	
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GPT-Link || Base Task || Functions")
	void ExecuteTask(const APawn* NPC, const FString& TaskSpecificData);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GPT-Link || Base Task || Functions")
	void GetTaskSpecificData(FString& TaskSpecificData);

	/**
	 * Returns the Task Name
	 *
	 * @returnThe Task Name
	 */
	UFUNCTION(BlueprintPure, Category = "GPT-Link || Base Task || Functions")
	FString GetTaskName() const { return TaskName_Internal; }


	UPROPERTY(BlueprintAssignable, Category = "GPT-Link || Base Task || Delegates")
	FExecuableTaskSignature OnExecutionPromtReceived;
	
	/**
	 * The Task information that will be sent to the GPT API
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GPT-Link || Base Task")
	FExecutableTask_S ExecutableTask;
	

	UFUNCTION()
	virtual void ExecuteTask_Internal(const APawn* NPC, FString TaskSpecificData);
	
	UFUNCTION()
	virtual void GetTaskSpecificData_Internal(FString& TaskSpecificData);
	
	bool RegisterSelfToGPTManager(TObjectPtr<AGPTLinkManager> GPTManager, FString TaskName, ETaskType NewTaskType);
	bool UnregisterSelfFromGPTManager();
	ETaskStatus GetTaskStatus() const;
	
	template<typename T>
	FString ConvertStructureToJSONString(T Task) const
	{
		FString TaskJsonObject;
		FJsonObjectConverter::UStructToJsonObjectString(T::StaticStruct(), &Task, TaskJsonObject);
		return TaskJsonObject;
	}
	
	bool HasNoTaskJob() const
	{
		return TaskType == ETaskType::None;
	}
	
	ETaskType TaskType = ETaskType::None;
	FExecuableTaskSignature TaskDelegate;
	
private:
	void SetTaskType(ETaskType NewTaskType);
	TObjectPtr<AGPTLinkManager> GPTManager;
	FString TaskName_Internal;
};