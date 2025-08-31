// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/*
* LSGameplayTags
* - Singleton containing nativce Gameplay tags
*/

struct FLSGameplayTags
{
public:
	/*
	* static methods
	*/
	static const FLSGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	/*
	* member methods
	*/
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddAllTags(UGameplayTagsManager& Manager);

	/*
	* �Ʒ��� GameplayTags�� �ʱ�ȭ �����ܰ�
	* - GameInstance�� �ʱ�ȭ ������ UGameFrameworkComponentManager�� RegisterInitiState�� ��ϵǾ� Linear�ϰ� ������Ʈ ��.
	* - �� �ʱ�ȭ GameplayTag�� ������ Actor ���̿� �����Ǹ�, GameFrameworkInitStateInterface ��ӹ��� Ŭ������ �ʱ�ȭ ����(Init State)�� ���¸ӽ�(State Machine)�� ���� ���� ������ �������̽��� �����Ѵ�
	*/
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;
	
	/* Enhanced Input Tags*/
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;

private:
	// static ���� �ʱ�ȭ�� .cpp�� ���ֱ�
	static FLSGameplayTags GameplayTags;
};