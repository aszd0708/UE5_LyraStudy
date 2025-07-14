// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraStudy.h"
#include "LSLogChannels.h"
#include "Modules/ModuleManager.h"

class FLyraStudyModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FLyraStudyModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();
	UE_LOG(LogLS, Warning, TEXT("FLyraStudyModule"))
}

void FLyraStudyModule::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FLyraStudyModule, LyraStudy, "LyraStudy");
