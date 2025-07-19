// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "LSAssetManager.generated.h"

/**
 *
 */
UCLASS()
class LYRASTUDY_API ULSAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	ULSAssetManager();

	static ULSAssetManager& Get();

	/*
	해당 함수는 FScopeLogTime을 사용할지 않을지 유무 판단을 위해 사용
	- 굳이 클론코딩할 필요 없지만, 알아두면 좋은 점들이 많다.
	*/
	static bool ShouldLogAssetLoads();

	virtual void StartInitialLoading() override;
	
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	/*
	GetAsset은 정적 로딩으로 BP Class와 Object 로딩
	- 주목해야 할 부분은 bKeeepInMemory 
	*/
	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubClass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);
	
	// GC의 대상
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Object 단위 Locking
	FCriticalSection SyncObject;
};

template <typename AssetType>
AssetType* ULSAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}
	} 

	if (LoadedAsset && bKeepInMemory)
	{
		// 여기서 AddLoadAsset은 메모리에 상주하기 위한 장피라고 생각하면 됨
		// - 한번 등록되면 직접 내리지 않는 한, Unlead가 되지 않음 (== 캐싱)
		Get().AddLoadAsset(Cast<UObject>(LoadedAsset));
	}
	
	return LoadedAsset;
}

template<typename AssetType>
inline TSubclassOf<AssetType> ULSAssetManager::GetSubClass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadSubclass;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadSubclass = AssetPointer.Get();
		if (!LoadSubclass)
		{
			LoadSubclass = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadSubclass, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}
	}

	if (LoadSubclass && bKeepInMemory)
	{
		Get().AddLoadAsset(Cast<UObject>(LoadSubclass));
	} 

	return LoadSubclass;
}

/*
Asset :  Blueprint, Sound
TSubclassOf : 특정 클래스의 서브클래스의 타입 정보
StaticClass : 메타데이터 (리플렉션) -> UClass
CDO : 클래스의 기본 인스턴스
*/