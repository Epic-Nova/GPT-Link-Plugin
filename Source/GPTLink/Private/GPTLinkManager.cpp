/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/

#include "GPTLinkManager.h"

#include "Components/NPCDefinition.h"
#include "Tasks/GPTLinkMoveToLocationTask.h"
#include "Tasks/GPTLinkSelectNextInteractionTask.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

void AGPTLinkManager::BeginPlay()
{
	Super::BeginPlay();

	HttpModule = &FHttpModule::Get();
	
	GPTLinkSettings = GetMutableDefault<UGPTLinkSettings>();
	
	for(const TPair<FString, TSubclassOf<UGPTLinkBaseTask>>& ElementPair : GPTLinkSettings->ExecutableTasks)
	{
		if(IsTaskRegistered(ElementPair.Key))
		{
			UE_LOG(LogGPTLink, Warning, TEXT("Task %s already registered!"), *ElementPair.Key);
		}
		else
		{
			UGPTLinkBaseTask* Task = NewObject<UGPTLinkBaseTask>(this, ElementPair.Value);
			if(!Task->RegisterSelfToGPTManager(this, ElementPair.Key, ETaskType::Blueprint))
			{
				UE_LOG(LogGPTLink, Warning, TEXT("Unknown Error on Task registration for: %s"), *ElementPair.Key)
			}
		}
	}
	RegisterDefaultTasks();
	SetStepsToPredict(0);
	RemoveUnregisteredTasksFromMemory();
}

void AGPTLinkManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	TArray<UGPTLinkBaseTask*> TasksToDestroy;
	for(const TPair<FString, UGPTLinkBaseTask*>& ElementPair : ExecutableTasks)
	{
		if(!IsTaskRegistered(ElementPair.Key))
		{
			UE_LOG(LogGPTLink, Warning, TEXT("Task %s not registered!"), *ElementPair.Key);
			continue;
		}
		TasksToDestroy.Add(ElementPair.Value);
	}

	for (UGPTLinkBaseTask* ToDestroy : TasksToDestroy)
	{
		if(ToDestroy->TaskType == ETaskType::Default) continue;
		ToDestroy->UnregisterSelfFromGPTManager();
	}
}

AGPTLinkManager* AGPTLinkManager::GetGPTLinkManager(const UObject* WorldContextObject)
{
	AActor* GPTLinkManager = UGameplayStatics::GetActorOfClass(WorldContextObject->GetWorld(), StaticClass());
	if(!GPTLinkManager)
	{
		UE_LOG(LogGPTLink, Log, TEXT("Spawning GPTLinkManager..."));
		GPTLinkManager = WorldContextObject->GetWorld()->SpawnActor<AGPTLinkManager>();
	}
	return Cast<AGPTLinkManager>(GPTLinkManager);
}

void AGPTLinkManager::RuntimeRegisterTask(FString TaskName, UGPTLinkBaseTask* TaskObject)
{
	if(!TaskObject) return;
	if(!IsTaskRegistered(TaskName))
	{
		if(!TaskObject->RegisterSelfToGPTManager(this, TaskName, ETaskType::Runtime))
		{
			UE_LOG(LogGPTLink, Warning, TEXT("Task %s not registered!"), *TaskName);
		}
	}
}

void AGPTLinkManager::RuntimeUnregisterTask(UGPTLinkBaseTask* Task)
{
	Task->UnregisterSelfFromGPTManager();
}

bool AGPTLinkManager::RegisterControlledPawn(APawn* Pawn)
{
	if(!Pawn) return false;

	UNPCDefinition* Component = NewObject<UNPCDefinition>(Pawn);
	Component->RegisterComponent();
	Pawn->AddInstanceComponent(Component);
	
	Pawn->OnDestroyed.AddDynamic(this, &AGPTLinkManager::OnPawnDestroyed);
	RegisteredControlledPawns.Add(Pawn->GetFName(), Pawn);

	UE_LOG(LogGPTLink, Log, TEXT("Registered Controlled Pawn: %s"), *Pawn->GetName());
	return true;
}

bool AGPTLinkManager::UnregisterControlledPawn(APawn* Pawn)
{
	if(!Pawn || !IsControlledPawnRegistered(Pawn)) return false;
	
	UActorComponent* Component = Pawn->GetComponentByClass(UNPCDefinition::StaticClass());
	if(Component == nullptr) return false;
	Pawn->RemoveInstanceComponent(Component);
	
	RegisteredControlledPawns.Remove(Pawn->GetFName());
	return true;
}

bool AGPTLinkManager::IsControlledPawnRegistered(const APawn* Pawn) const
{
	if(!Pawn) return false;
	return RegisteredControlledPawns.Contains(Pawn->GetFName());
}

TMap<FName, APawn*> AGPTLinkManager::GetAllControlledPawns() const
{
	return RegisteredControlledPawns;
}

bool AGPTLinkManager::IsTaskRegistered(FString TaskName) const
{
	return ExecutableTasks.Contains(TaskName) && ExecutableTasks[TaskName] != nullptr;
}

UGPTLinkBaseTask* AGPTLinkManager::GetTask(FString TaskName) const
{
	if(!IsTaskRegistered(TaskName))
	{
		UE_LOG(LogGPTLink, Warning, TEXT("Task %s not registered!"), *TaskName);
		return nullptr;
	}
	return ExecutableTasks[TaskName];		
}

TArray<UGPTLinkBaseTask*> AGPTLinkManager::GetAllTasks() const
{
	TArray<UGPTLinkBaseTask*> Tasks;
	ExecutableTasks.GenerateValueArray(Tasks);
	return Tasks;
}

void AGPTLinkManager::DecideTasks(FString SinglePawnOverride)
{
	FString JsonString;
	const TSharedPtr<FJsonObject> ExecutableTasksObject = FJsonObjectConverter::UStructToJsonObject(GatherTaskParameters(SinglePawnOverride));
	
	for (const TPair<FString, UGPTLinkBaseTask*> ExecutableTask : ExecutableTasks)
	{
		if(!IsTaskRegistered(ExecutableTask.Key)) continue;

		FString TaskSpecificData;
		ExecutableTask.Value->GetTaskSpecificData_Internal(TaskSpecificData);

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(TaskSpecificData);
		FJsonSerializer::Deserialize(Reader, JsonObject);
		
		ExecutableTasksObject->GetObjectField("executableTasks")->GetObjectField(ExecutableTask.Key)->SetObjectField("TaskSpecificData", JsonObject);
	}
	
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(ExecutableTasksObject.ToSharedRef(), Writer);
	
	const TSharedRef<IHttpRequest> Request = HttpModule->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AGPTLinkManager::OnResponseReceived);
	Request->SetURL(GPTLinkSettings->APIEndpoint);
	Request->SetVerb("POST");
	Request->SetHeader("Authorization", "Bearer " + GPTLinkSettings->APIKey);
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetContentAsString(JsonString);
	Request->ProcessRequest();
}

void AGPTLinkManager::SetStepsToPredict(int32 StepsToPredict)
{
	if(StepsToPredict <= 0)
	{
		StepsToPredict_Internal = GPTLinkSettings->DefaultStepsToPredict;
		return;
	}
	StepsToPredict_Internal = StepsToPredict;
}

void AGPTLinkManager::RemoveUnregisteredTasksFromMemory() const
{
	TObjectIterator<UGPTLinkBaseTask> TaskIterator;
	while(TaskIterator)
	{
		if(!IsTaskRegistered(TaskIterator->GetTaskName()))
		{
			FString TaskName = TaskIterator->GetTaskName().IsEmpty() ? TaskIterator->GetName() : *TaskIterator->GetTaskName();
			UE_LOG(LogGPTLink, Log, TEXT("Removing unused task %s from memory..."), *TaskName);
			TaskIterator->ConditionalBeginDestroy();
		}
		++TaskIterator;
	}
}

void AGPTLinkManager::ExecuteTask(const UGPTLinkBaseTask* GPTLinkTask, const FString NPCIdentifier, FString TaskSpecificData)
{
	if(!GPTLinkTask) return;
	if(GPTLinkTask->HasNoTaskJob())
	{
		UE_LOG(LogGPTLink, Error, TEXT("Task %s has no job!"), *GPTLinkTask->GetTaskName());
		return;
	}
	if(!RegisteredControlledPawns.Contains(*FString(NPCIdentifier)))
	{
		UE_LOG(LogGPTLink, Error, TEXT("NPC %s is not registered!"), *NPCIdentifier);
		return;	
	}
	const APawn* ControlledPawn = RegisteredControlledPawns[*FString(NPCIdentifier)];

	UActorComponent* NPCComponent = ControlledPawn->GetComponentByClass(UNPCDefinition::StaticClass());
	if(NPCComponent == nullptr) return;

	UNPCDefinition* CastedNPCComponent = Cast<UNPCDefinition>(NPCComponent);
	if(CastedNPCComponent == nullptr) return;

	if(!CastedNPCComponent->bHasFirstTaskBeenExecuted)
	{
		CastedNPCComponent->OriginTask = GPTLinkTask->GetTaskName();
		CastedNPCComponent->bHasFirstTaskBeenExecuted = true;
	}
	CastedNPCComponent->PreviousTask = CastedNPCComponent->CurrentTask;
	CastedNPCComponent->CurrentTask = GPTLinkTask->GetTaskName();
	
	GPTLinkTask->TaskDelegate.Broadcast(RegisteredControlledPawns[*FString(NPCIdentifier)], TaskSpecificData);
}

void AGPTLinkManager::RegisterDefaultTasks()
{
	// GPTLinkMoveToLocationTask
	{
		UGPTLinkMoveToLocationTask* Task = NewObject<UGPTLinkMoveToLocationTask>(this);
		Task->RegisterSelfToGPTManager(this, "MoveToLocation", ETaskType::Default);
	}
	// GPTLinkSelectNextInteractionTask
	{
		UGPTLinkSelectNextInteractionTask* Task = NewObject<UGPTLinkSelectNextInteractionTask>(this);
		Task->RegisterSelfToGPTManager(this, "SelectNextInteraction", ETaskType::Default);
	}
}

void AGPTLinkManager::OnPawnDestroyed(AActor* DestroyedActor)
{
	if(!DestroyedActor) return;
	if(!DestroyedActor->IsA<APawn>()) return;
	APawn* DestroyedPawn = Cast<APawn>(DestroyedActor);
	if(!DestroyedPawn) return;
	if(!IsControlledPawnRegistered(DestroyedPawn))
	{
		UE_LOG(LogGPTLink, Warning, TEXT("Pawn %s not registered!"), *DestroyedPawn->GetFName().ToString());
		return;
	}
	UnregisterControlledPawn(DestroyedPawn);
}

FGPTRequestData_S AGPTLinkManager::GatherTaskParameters(FString SinglePawnOverride) const
{
	FGPTRequestData_S RequestData;

	TArray<FGPTRequestNPCData_S> RequestNPCData;
	if(SinglePawnOverride.IsEmpty())
	{
		UE_LOG(LogGPTLink, Log, TEXT("Gathering task parameters for all controlled pawns..."));
		
		for (const TPair<FName, APawn*> NPCData : GetAllControlledPawns())
		{
			UActorComponent* NPCComponent = NPCData.Value->GetComponentByClass(UNPCDefinition::StaticClass());
			if(NPCComponent == nullptr) continue;

			UNPCDefinition* CastedNPCComponent = Cast<UNPCDefinition>(NPCComponent);
			if(CastedNPCComponent == nullptr) continue;
			
			if(!IsControlledPawnRegistered(NPCData.Value)) continue;
			FGPTRequestNPCData_S RequestNPCDataElement;
			RequestNPCDataElement.CurrentPosition = NPCData.Value->GetActorLocation();
			RequestNPCDataElement.OriginTask = CastedNPCComponent->OriginTask;
			RequestNPCDataElement.PreviousTask = CastedNPCComponent->PreviousTask;
			RequestNPCDataElement.NPCIdentifier = NPCData.Key;
			
			RequestNPCData.Add(RequestNPCDataElement);
		}
		RequestData.NPCData = RequestNPCData;
	}
	else if(RegisteredControlledPawns.Contains(*SinglePawnOverride))
	{
		UE_LOG(LogGPTLink, Log, TEXT("Gathering task parameters for single pawn %s..."), *SinglePawnOverride);
		
		APawn* SinglePawn = RegisteredControlledPawns[*SinglePawnOverride];
		
		if(!SinglePawn)
		{
			UE_LOG(LogGPTLink, Error, TEXT("Pawn %s is not valid!"), *SinglePawnOverride);
		}
		
		UActorComponent* NPCComponent = SinglePawn->GetComponentByClass(UNPCDefinition::StaticClass());
		if(NPCComponent == nullptr) return FGPTRequestData_S();

		UNPCDefinition* CastedNPCComponent = Cast<UNPCDefinition>(NPCComponent);
		if(CastedNPCComponent == nullptr) return FGPTRequestData_S();
		
		FGPTRequestNPCData_S SingleNPCData;
		SingleNPCData.CurrentPosition = SinglePawn->GetActorLocation();
		SingleNPCData.OriginTask = CastedNPCComponent->OriginTask;
		SingleNPCData.PreviousTask = CastedNPCComponent->PreviousTask;
		SingleNPCData.NPCIdentifier = SinglePawn->GetFName();

		RequestNPCData.Add(SingleNPCData);
	}
	else
	{
		UE_LOG(LogGPTLink, Error, TEXT("Pawn %s is not registered!"), *SinglePawnOverride);
		return FGPTRequestData_S();
	}

	if(GPTLinkSettings && GPTLinkSettings->bDebugMode)
	{
		FString JsonString;	
		FJsonObjectConverter::UStructToJsonObjectString(RequestData, JsonString);
		UE_LOG(LogGPTLink, Log, TEXT("Request data: %s"), *JsonString);
	}
	
	TMap<FString, FExecutableTask_S> ExecutableTasksMap;
	for (const TPair<FString, UGPTLinkBaseTask*> ExecutableTaskElement : ExecutableTasks)
	{
		ExecutableTasksMap.Add(ExecutableTaskElement.Key, ExecutableTaskElement.Value->ExecutableTask);
	}
	RequestData.ExecutableTasks = ExecutableTasksMap;

	//RequestData.StepsToPredict = StepsToPredict_Internal;  //@TODO
	
	return RequestData;
}

void AGPTLinkManager::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(GPTLinkSettings && GPTLinkSettings->bDebugMode)
	{
		UE_LOG(LogGPTLink, Log, TEXT("Response received, and copied to clipboard!"));
		FPlatformApplicationMisc::ClipboardCopy(*Response->GetContentAsString());
	}

	TSharedPtr<FJsonValue> JsonValue;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    if (FJsonSerializer::Deserialize(Reader, JsonValue))
    {
	    if(const TArray<TSharedPtr<FJsonValue>>* NPCData; JsonValue->AsObject()->TryGetArrayField("NPCData", NPCData))
    	{
	        for (const TSharedPtr<FJsonValue> NPC : *NPCData)
	        {
		        const TSharedPtr<FJsonObject> NPCObject = NPC->AsObject();
        		FString NPCIdentifier = NPCObject->GetStringField("NPCIdentifier");
		        const TSharedPtr<FJsonObject> NextTask = NPCObject->GetObjectField("NextTask");

		        const FString TaskIdentifier = NextTask->GetStringField("Task");
        		TSharedPtr<FJsonObject> TaskParameters = NextTask->GetObjectField("TaskParameters");

        		if(!IsTaskRegistered(TaskIdentifier)) continue;

		        const UGPTLinkBaseTask* Task = GetTask(TaskIdentifier);
        		if(!Task) continue;

        		FString TaskParametersString;
        		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&TaskParametersString);
        		FJsonSerializer::Serialize(TaskParameters.ToSharedRef(), Writer);

	        	if(GPTLinkSettings && GPTLinkSettings->bDebugMode)
	        	{
	        		UE_LOG(LogGPTLink, Log, TEXT("Task Specific Data are present: %s"), *TaskParametersString);
	        	}
	        	
        		if(RegisteredControlledPawns.Contains(*NPCIdentifier))
        		{
        			UE_LOG(LogGPTLink, Log, TEXT("Executing task %s on NPC: %s"), *TaskIdentifier, *NPCIdentifier);
        			OnTaskExecuted.Broadcast(RegisteredControlledPawns[*NPCIdentifier], TaskParametersString);
        			ExecuteTask(Task, NPCIdentifier, TaskParametersString);
        		}
	            else
	            {
		            UE_LOG(LogGPTLink, Error, TEXT("Tried to execute task %s on unregistered NPC %s"), *TaskIdentifier, *NPCIdentifier)
	            }
	        }
    	}
        else
        {
	        UE_LOG(LogGPTLink, Error, TEXT("Failed to parse response! Retrying..."));
        	DecideTasks("");
        }
    }

	//@TODO: Program this Absolute thread safe, due to the ability of the AI to response with non valid data
}
