// Fill out your copyright notice in the Description page of Project Settings.


#include "LyraStudy/LSLogChannels.h"
#include "System/LSAssetManager.h"

ULSAssetManager::ULSAssetManager()
{
}

ULSAssetManager& ULSAssetManager::Get()
{
	check(GEngine);

	// �츮�� AssetManager�� UEngine�� GEninge�� AssetManager�� Class�� �������̵� �߱� ������ GEngine�� AssetManager�� ����
	if (ULSAssetManager* Singleton = Cast<ULSAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogLS, Fatal, TEXT("invalied AssetmanagerClassName in DefaultEngine.ini(project Settings); it must be LSAssetManager"));

	// ���� UE_LOG�� Fatal�� ����, Crash ���� ������ �������� ������, �������� ���� ���̷� ����
	return *NewObject<ULSAssetManager>();
}

bool ULSAssetManager::ShouldLogAssetLoads()
{
	const TCHAR* CommandLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(CommandLineContent, TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

UObject* ULSAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	// �ش� �Լ��� ���� ���� ������ ' synchronous load asset�� ���ʿ��ϰ� �ϴ� ���� ������ Ȯ���ϱ� ����.
	if (AssetPath.IsValid())
	{
		// FScopeLogTime�� Ȯ���غ���
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			// �ܼ��� �α��ϸ鼭, �ʴ����� �α� ����
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronouse loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		// ���⼭ �ΰ��� �б�:
		// 1. AssetManager�� ������, AssetManager�� StreamableManager�� ���� ���� �ε�
		// 2. �ƴϸ�,  FSoftObjectPath�� ���� �ٷ� ���� �ε�
		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}
	}

	// if asset manager is no ready, use LoadObject()
	// - �� ����, StaticLoadObject�� ���δ�.
	// - �����, �׻� StaticLoadObject �ϱ� ���� StaticFindObject�� ���� Ȯ���ϰ� �����ϸ� ��¥ �ε���
	return AssetPath.TryLoad();
}
