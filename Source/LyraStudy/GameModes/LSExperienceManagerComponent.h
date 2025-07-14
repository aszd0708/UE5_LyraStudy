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
 * - �� �״��, �ش� component�� game state�� owner�� �����鼭, experience�� ���� ������ ������ �ִ� component�̴�.
 * - �Ӹ� �ƴ϶�, amanger��� �ܾ ���ԵǾ� �ֵ���, experience �ε� ���� ������Ʈ �� �̺�Ʈ�� �����Ѵ�.
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
	* �Ʒ��� OnExperienceLoaded�� ���ε� �ϰų�, �̹� Experience �ε��� �Ϸ� �Ǿ��ٸ� �ٷ� ȣ����
	*/
	void CallOrRegister_OnExperiencedLoaded(FOnLSExperienceLoaded::FDelegate&& Delegate);

public:
	UPROPERTY()
	TObjectPtr<const ULSExperienceDefinition> CurrentExperience;

	/* Experience�� �ε� ���¸� ����͸� */
	ELSExperienceLoadState LoadState = ELSExperienceLoadState::Unloaded;

	/* Experience �ε��� �Ϸ�� ����, Broadcasting Delegate */
	FOnLSExperienceLoaded OnExperienceLoaded;
};
