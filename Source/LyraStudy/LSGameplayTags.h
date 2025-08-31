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
	* 아래의 GameplayTags는 초기화 과정단계
	* - GameInstance의 초기화 과정에 UGameFrameworkComponentManager의 RegisterInitiState로 등록되어 Linear하게 업데이트 됨.
	* - 이 초기화 GameplayTag는 게임의 Actor 사이에 공유되며, GameFrameworkInitStateInterface 상속받은 클래스는 초기화 상태(Init State)를 상태머신(State Machine)과 같이 관리 가능한 인터페이스를 제공한다
	*/
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;
	
	/* Enhanced Input Tags*/
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;

private:
	// static 변수 초기화는 .cpp에 해주기
	static FLSGameplayTags GameplayTags;
};