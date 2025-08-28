// Fill out your copyright notice in the Description page of Project Settings.


#include "LSExperienceManagerComponent.h"
#include "GameFeaturesSubsystemSettings.h"
#include "System/LSAssetManager.h"
#include "LSExperienceDefinition.h"
#include "LSLogChannels.h"

void ULSExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnLSExperienceLoaded::FDelegate&& Delegate)
{
	// IsExperienceLoaded() ����
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		/*
		* �����, Delegate ��ü�� ���캸��, ���������� �ʿ��� �������� �޸� �Ҵ��� ���´�.
		* TArray<int> a = {1,2,3,4};
		* delegate_type delegate = [a](){
		* return a.Num();
		* }
		* a�� delegate_type ���ο� new�� �Ҵ�Ǿ� �ִ�. ���� ����� ���߱� ���� Move �� ���� �ϴ� ���� ���� ����
		*/
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void ULSExperienceManagerComponent::ServerSetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	ULSAssetManager& AssetManager = ULSAssetManager::Get();

	TSubclassOf<ULSExperienceDefinition> AssetClass;
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
		AssetClass = Cast<UClass>(AssetPath.TryLoad());
	}

	// �� CDO(Class Default Object)�� �����ñ�
	const ULSExperienceDefinition* Experience = GetDefault<ULSExperienceDefinition>(AssetClass);
	check(Experience != nullptr);
	check(CurrentExperience == nullptr);
	{
		// �׸��� CDO�� CurrentExperience�� �����Ѵ�!
		// � �ǵ��� �̷��� �ڵ带 �ۼ������� �ڵ带 �� �о��(StartExperienceLoad����) �ٽ� ����
		CurrentExperience = Experience;
	}

	StartExperienceLoad();
}

PRAGMA_DISABLE_OPTIMIZATION
void ULSExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	check(LoadState == ELSExperienceLoadState::Unloaded);

	LoadState = ELSExperienceLoadState::Loading;

	ULSAssetManager& AssetManager = ULSAssetManager::Get();

	// �̹� �ռ�, ServerSetCurrentExperience���� �츮�� ExperienceId�� �Ѱ��־��µ�, ���⼭ CDO�� Ȱ���Ͽ�, GetPrimaryAssetId�� �ε��� ������� �ִ´�.
	// �� �̷���??
	// - GetPrimaryAssetId�� �� �� �ڼ��� ����
	// - GetPrimaryAssetId�� ���캽���ν�, �Ʒ��� �ΰ����� �� �� �ִ�.
	// 1. �츮�� B_LSDefaultExperience�� BP�� ���� ����
	// 2. CDO�� �����ͼ�, GetPrimaryAssetId�� ȣ���� ����

	// �츮�� �ռ� �̹� CDO�� �ε��Ͽ�, CDO�� ������� �ʰ� CDO�� ����Ͽ� �ε��� ������ �����Ͽ�, BundleAssetList�� �ش�.
	TSet<FPrimaryAssetId> BundleAssetList;
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	// load assets associated with the experience
	// �Ʒ��� �츮�� ���� GameFeature�� ����Ͽ�, Experience�� ���ε� ��, GameFeature Plaugin�� �ε��� Bundle�̸��� �߰����Ѵ�.
	// - Bundle �̶�°� ���� �츮�� �ε��� ������ ī�׷α� �̸��̶�� �����ϸ� �ȴ�
	TArray<FName> BundlesToLoad;
	{
		// ���⼭ �ָ��ؾ� �� �κ��� OwnerNetMode�� NM_Standalone�̸�? Client/Server �Ѵ� �ε��� �߰��ȴ�.
		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
		bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
		bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
		if (bLoadClient)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
		}
		if (bLoadServer)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
		}
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);
	// �Ʒ���, ���� Bundle�� �츮�� GameFeature�� �����ϸ鼭 �� ��� �˾ƺ���, ������ B_LSDefaultExperience�� �ε����ִ� �Լ��� ����
	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);
	
	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		// �ε��� �Ϸ�Ǿ�����, ExecuteDelegate�� ���� OnAssetsLoadedDelegate�� ȣ������:
		// - �Ʒ��� �Լ��� Ȯ���غ���:
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
			{
				OnAssetsLoadedDelegate.ExecuteIfBound();
			}));
	}

	// FrameNumber�� �ָ��ؼ� ����
	static int32 StartExperienceLoad_FrameNumber = GFrameNumber;
}
PRAGMA_ENABLE_OPTIMIZATION

void ULSExperienceManagerComponent::OnExperienceLoadComplete()
{
	// FrameNumber�� �ָ��ؼ� ����
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	// �ش� �Լ��� �Ҹ��� ���� �ռ� ���Ҵ� StreamableDelegateDelayHelper�� ���� �Ҹ�
	OnExperienceFullLoadCompleted();
}

void ULSExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != ELSExperienceLoadState::Loaded);

	LoadState = ELSExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

const ULSExperienceDefinition* ULSExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == ELSExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}
