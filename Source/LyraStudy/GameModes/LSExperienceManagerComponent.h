// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "LSExperienceManagerComponent.generated.h"

class ULSExperienceDefinition;

enum class ELSExperienceLoadState
{
	Unloaded,
	Loading,
	Loaded,
	Deactivating,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLSExperienceLoaded, const ULSExperienceDefinition*);
/**
 * LSExperienceManagerComponent
 * - 말 그대로, 해당 component는 game state를 owner로 가지면서, experience의 상태 정보를 가지고 있는 component이다.
 * - 뿐만 아니라, amanger라는 단어가 포함되어 있듯이, experience 로딩 상태 업데이트 및 이벤트르 관리한다.
 */
UCLASS()
class LYRASTUDY_API ULSExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	/*
	* memeber methods
	*/
	bool IsExperienceLoaded() { return (LoadState == ELSExperienceLoadState::Loading) && (CurrentExperience != nullptr); }

	/*
	* 아래의 OnExperienceLoaded에 바인딩 하거나, 이미 Experience 로딩이 완료 되었다면 바로 호출함
	*/
	void CallOrRegister_OnExperiencedLoaded(FOnLSExperienceLoaded::FDelegate&& Delegate);

public:
	UPROPERTY()
	TObjectPtr<const ULSExperienceDefinition> CurrentExperience;

	/* Experience의 로딩 상태를 모니터링 */
	ELSExperienceLoadState LoadState = ELSExperienceLoadState::Unloaded;

	/* Experience 로딩이 완료된 이후, Broadcasting Delegate */
	FOnLSExperienceLoaded OnExperienceLoaded;
};
