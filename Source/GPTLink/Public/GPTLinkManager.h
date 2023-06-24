/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/
#pragma once

#include "GPTLinkSettings.h"
#include "GPTLinkTypes.h"
#include "Http.h"
#include "GPTLinkManager.generated.h"

UCLASS(DisplayName = "GPT Manager")
class GPTLINK_API AGPTLinkManager : public AActor
{
	GENERATED_BODY()

public:
	
	/**
	 * Returns an Instance of the GPT-Link Manager, when the Manager is not found, it will be created	and spawned at the World Origin
	 *
	 * @return The Instance of the GPT-Link Manager
	 */
	UFUNCTION(BlueprintPure, Category = "GPT-Link || Manager || Functions", meta = (WorldContext = "WorldContextObject"))
	static AGPTLinkManager* GetGPTLinkManager(const UObject* WorldContextObject);

	/**
	 * Registers an Task at Runtime
	 *
	 * @param TaskName The name that should be used for the Key in the C++ Map
	 * @param TaskObject The object that should be registered with the associated name
	 */
	UFUNCTION(BlueprintCallable, Category = "GPT-Link || Manager || Functions")
	void RuntimeRegisterTask(FString TaskName, UGPTLinkBaseTask* TaskObject);

	/**
	 * Unregisters an Task at Runtime
	 *
	 * @param Task The object that should be unregistered
	 */
	UFUNCTION(BlueprintCallable, Category = "GPT-Link || Manager || Functions")
	void RuntimeUnregisterTask(UGPTLinkBaseTask* Task);

	/**
	 * Checks if an Task is registered
	 *
	 * @param TaskName The name of the Task that should be checked
	 * @return True if the Task is registered, false if not
	 */
	UFUNCTION(BlueprintPure, Category = "GPT-Link || Manager || Functions")
	bool IsTaskRegistered(FString TaskName) const;

	/**
	 * This function sends a request to the GPT-3 API, and returns a set of decisions that will then be executed.
	 */
	UFUNCTION(BlueprintCallable, Category = "GPT-Link || Manager || Functions")
	void DecideTasks(FString SinglePawnOverride);

	/**
	 * Returns the Task that is registered with the given name
	 *
	 * @param TaskName The name of the Task that should be returned
	 * @return The Task that is registered with the given name, nullptr if not registered
	 */
	UFUNCTION(BlueprintPure, Category = "GPT-Link || Manager || Functions")
	UGPTLinkBaseTask* GetTask(FString TaskName) const;

	/**
	 * Returns all registered Tasks
	 *
	 * @return An array with all registered Tasks
	 */
	UFUNCTION(BlueprintPure, Category = "GPT-Link || Manager || Functions")
	TArray<UGPTLinkBaseTask*> GetAllTasks() const;
	
	/**
	 * Registers an NPC for ChatGPT to control
	 *
	 * @param Pawn The Pawn that should be registered
	 * @return True if the Pawn was registered, false if not
	 */
	UFUNCTION(BlueprintCallable, Category = "GPT-Link || Manager || Functions")
	bool RegisterControlledPawn(APawn* Pawn);

	/**
	 * Unregisters an NPC so ChatGPT will no longer control it
	 *
	 * @param Pawn The Pawn that should be unregistered
	 * @return True if the Pawn was unregistered, false if not
	 */
	UFUNCTION(BlueprintCallable, Category = "GPT-Link || Manager || Functions")
	bool UnregisterControlledPawn(APawn* Pawn);

	/**
	 * Checks if an NPC with the given reference is registered
	 *
	 * @param Pawn The Pawn that should be checked
	 * @return True if the Pawn is registered, false if not
	 */
	UFUNCTION(BlueprintPure, Category = "GPT-Link || Manager || Functions")
	bool IsControlledPawnRegistered(const APawn* Pawn) const;

	/**
	 * Returns all registered NPCs
	 *
	 * @return A map with all registered NPCs, the key is the name of the NPC
	 */
	UFUNCTION(BlueprintPure, Category = "GPT-Link || Manager || Functions")
	TMap<FName, APawn*> GetAllControlledPawns() const;

	/**
	 * Sets the number of steps that should be predicted from ChatGPT
	 *
	 * @param StepsToPredict The number of steps that should be predicted
	 */
	UFUNCTION(BlueprintCallable, Category = "GPT-Link || Manager || Functions", meta = (DeprecatedFunction, DeprecationMessage = "The Prediction system will be replaced by a new one in the future"))
	void SetStepsToPredict(int32 StepsToPredict);
	
	/**
	 * All registered Tasks that can be executed by ChatGPT
	 */
	UPROPERTY(Transient, VisibleInstanceOnly, Instanced, Category = "GPT-Link")
	TMap<FString, UGPTLinkBaseTask*> ExecutableTasks;
	
	void RemoveUnregisteredTasksFromMemory() const;
	void ExecuteTask(const UGPTLinkBaseTask* GPTLinkTask, const FString NPCIdentifier, FString TaskSpecificData);
	
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	FExecuableTaskSignature OnTaskExecuted;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);
	void RegisterDefaultTasks();
	FGPTRequestData_S GatherTaskParameters(FString SinglePawnOverride) const;
	
	UPROPERTY(Transient)
	TMap<FName, APawn*> RegisteredControlledPawns;
	TObjectPtr<UGPTLinkSettings> GPTLinkSettings;
	int32 StepsToPredict_Internal;
	FHttpModule* HttpModule;
};
