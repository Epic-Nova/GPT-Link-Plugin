/*==========================================================================>
|				GPT-Link - ChatGPT interacting with Unreal Engine			|
| ------------------------------------------------------------------------- |
|					Copyright (C) 2023 Simsalabim Studios.					|
| ------------------------------------------------------------------------- |
<==========================================================================*/
#pragma once

#include "GPTLinkSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, DisplayName = "GPT-Link Settings")
class GPTLINK_API UGPTLinkSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere, Category = "GPT-Link", DisplayName = "OpenAI API Key")
	FString APIKey;

	/**
	 * The API is just the Translation layer for the generated JSON Objects from unreal,
	 * and GPT, so you can use your own API if you want to.
	 * The API can be found under:
	 * @link https://github.com/Epic-Nova/GPT-Link-API
	 */
	UPROPERTY(Config, EditAnywhere, Category = "GPT-Link")
	FString APIEndpoint = "http://gpt.epicnova.net";

	UPROPERTY(Config, VisibleAnywhere, Category = "GPT-Link")
	EGPTModel GPTModel;
	
	UPROPERTY(Config, EditAnywhere, Category = "GPT-Link")
	TMap<FString, TSubclassOf<UGPTLinkBaseTask>> ExecutableTasks;

	/**
	 * The prediction system is Deprecated, and will be replaced in the future.
	 */
	UPROPERTY(Config, VisibleAnywhere, Category = "GPT-Link")
	int32 DefaultStepsToPredict = 0;

	UPROPERTY(Config, EditAnywhere, Category = "GPT-Link")
	bool bDebugMode = false;

	UGPTLinkSettings() {};
};
