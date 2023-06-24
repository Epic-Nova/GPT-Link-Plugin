/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NPCDefinition.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GPTLINK_API UNPCDefinition : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNPCDefinition();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GPT-Link || NPC Definition")
	FString OriginTask;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GPT-Link || NPC Definition")
	FString PreviousTask;
	
	FString CurrentTask;

	bool bHasFirstTaskBeenExecuted = false;	
};
