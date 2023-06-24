/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/

#include "Tasks/GPTLinkBaseTask.h"
#include "GPTLinkManager.h"

void UGPTLinkBaseTask::ExecuteTask_Implementation(const APawn* NPC, const FString& TaskSpecificData)
{
	if(TaskType != ETaskType::Default)
	{
		UE_LOG(LogGPTLink, Warning, TEXT("Call to ExecuteTask base Implementation! Please override this function!"))
	}
}

void UGPTLinkBaseTask::GetTaskSpecificData_Implementation(FString& TaskSpecificData)
{
	UE_LOG(LogGPTLink, Warning, TEXT("Call to GetTaskSpecificData base Implementation! Please override this function!"))
}

void UGPTLinkBaseTask::ExecuteTask_Internal(const APawn* NPC, FString TaskSpecificData)
{
	OnExecutionPromtReceived.Broadcast(NPC, TaskSpecificData);
	ExecuteTask(NPC, TaskSpecificData);

	UE_LOG(LogGPTLink, Log, TEXT("Received Execution Promt for Task %s on NPC with Identifier: %s"), *TaskName_Internal, *NPC->GetHumanReadableName());
}

void UGPTLinkBaseTask::GetTaskSpecificData_Internal(FString& TaskSpecificData)
{
	return GetTaskSpecificData(TaskSpecificData);
}

bool UGPTLinkBaseTask::RegisterSelfToGPTManager(TObjectPtr<AGPTLinkManager> NewGPTManager, FString TaskName, ETaskType NewTaskType)
{
	GPTManager = NewGPTManager;
	TaskName_Internal = TaskName;
	
	const ETaskStatus TaskStatus = GetTaskStatus();
	
	SetTaskType(NewTaskType);
	
	if(TaskStatus != ETaskStatus::ReadyToInteract)
	{
		UE_LOG(LogGPTLink, Warning, TEXT("Task %s returned error status: %s"), *TaskName, *UEnum::GetValueAsString(TaskStatus));
		return false;
	}
	
	if(TaskStatus != ETaskStatus::AlreadyRegistered)
	{
		TaskDelegate.Clear();
		TaskDelegate.AddDynamic(this, &UGPTLinkBaseTask::ExecuteTask_Internal);
		if (!GPTManager->ExecutableTasks.Contains(TaskName))
		{
			GPTManager->ExecutableTasks.Add(TaskName, this);
			ExecutableTask.bIsDefaultTask = true;
		}
		UE_LOG(LogGPTLink, Log, TEXT("Task %s registered!"), *TaskName);
		return true;
	}
	return false;
}

bool UGPTLinkBaseTask::UnregisterSelfFromGPTManager()
{
	const ETaskStatus TaskStatus = GetTaskStatus();
	if(TaskStatus != ETaskStatus::AlreadyRegistered)
	{
		UE_LOG(LogGPTLink, Warning, TEXT("Task %s returned error status: %s"), *TaskName_Internal, *UEnum::GetValueAsString(TaskStatus));
		return false;
	}
	TaskDelegate.Clear();
	GPTManager->ExecutableTasks.Remove(TaskName_Internal);
	TaskType = ETaskType::None;
	UE_LOG(LogGPTLink, Log, TEXT("Task %s unregistered and destroyed!"), *TaskName_Internal);
	ConditionalBeginDestroy();
	return true;
}

ETaskStatus UGPTLinkBaseTask::GetTaskStatus() const
{
	if(!GPTManager)
	{
		UE_LOG(LogGPTLink, Error, TEXT("GPTManager not set!"));
		return ETaskStatus::GPTManagerNotInitialized;
	}
	if(GPTManager->IsTaskRegistered(TaskName_Internal))
	{
		return ETaskStatus::AlreadyRegistered;
	}
	if(TaskType == ETaskType::Default)
	{
		UE_LOG(LogGPTLink, Warning, TEXT("Task %s is not modifiable"), *TaskName_Internal);
		return ETaskStatus::NotModifiable;
	}
	return ETaskStatus::ReadyToInteract;
}

void UGPTLinkBaseTask::SetTaskType(ETaskType NewTaskType)
{
	if(!HasNoTaskJob())
	{
		UE_LOG(LogGPTLink, Error, TEXT("Task: %s already has a task type set, cannot set it again!"), *TaskName_Internal);
	}
	TaskType = NewTaskType;
	UE_LOG(LogGPTLink, Log, TEXT("Setting task type for Task: %s to: %s"), *TaskName_Internal, *UEnum::GetValueAsString(TaskType));
}
