// Fill out your copyright notice in the Description page of Project Settings.


#include "System/LSGameInstance.h"
#include "Components/GameFrameworkComponentManager.h"
#include "LSGameplayTags.h"

void ULSGameInstance::Init()
{
	Super::Init();

	UGameFrameworkComponentManager* ComponentManager = GetSubsystem< UGameFrameworkComponentManager>(this);
	if (ensure(ComponentManager)) 
	{
		const FLSGameplayTags& GameplayTags = FLSGameplayTags::Get();

		ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_DataAvailable);
		ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
	}
}

void ULSGameInstance::Shutdown()
{
	Super::Shutdown();
}
