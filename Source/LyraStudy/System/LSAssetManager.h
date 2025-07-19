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
	�ش� �Լ��� FScopeLogTime�� ������� ������ ���� �Ǵ��� ���� ���
	- ���� Ŭ���ڵ��� �ʿ� ������, �˾Ƶθ� ���� ������ ����.
	*/
	static bool ShouldLogAssetLoads();

	virtual void StartInitialLoading() override;
	
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	/*
	GetAsset�� ���� �ε����� BP Class�� Object �ε�
	- �ָ��ؾ� �� �κ��� bKeeepInMemory 
	*/
	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubClass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);
	
	// GC�� ���
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Object ���� Locking
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
		// ���⼭ AddLoadAsset�� �޸𸮿� �����ϱ� ���� ���Ƕ�� �����ϸ� ��
		// - �ѹ� ��ϵǸ� ���� ������ �ʴ� ��, Unlead�� ���� ���� (== ĳ��)
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
TSubclassOf : Ư�� Ŭ������ ����Ŭ������ Ÿ�� ����
StaticClass : ��Ÿ������ (���÷���) -> UClass
CDO : Ŭ������ �⺻ �ν��Ͻ�
*/