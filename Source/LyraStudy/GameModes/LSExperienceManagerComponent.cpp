// Fill out your copyright notice in the Description page of Project Settings.


#include "LSExperienceManagerComponent.h"
#include "GameFeaturesSubsystemSettings.h"
#include "System/LSAssetManager.h"
#include "LSExperienceDefinition.h"

void ULSExperienceManagerComponent::CallOrRegister_OnExperiencedLoaded(FOnLSExperienceLoaded::FDelegate&& Delegate)
{
	// IsExperienceLoaded() 구현
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		/*
		* 참고로, Delegate 객체를 살펴보면, 내부적으로 필요한 변수들은 메모리 할당해 놓는다.
		* TArray<int> a = {1,2,3,4};
		* delegate_type delegate = [a](){
		* return a.Num();
		* }
		* a는 delegate_type 내부에 new로 할당되어 있다. 복사 비용을 낮추기 위해 Move 를 통해 하는 것을 잊지 말자
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

	// 왜 CDO(Class Default Object)를 가져올까
	const ULSExperienceDefinition* Experience = GetDefault<ULSExperienceDefinition>(AssetClass);
	check(Experience != nullptr);
	check(CurrentExperience == nullptr);
	{
		// 그리고 CDO로 CurrentExperience를 설정한다!
		// 어떤 의도로 이렇게 코드를 작성한지는 코드를 쭉 읽어보고(StartExperienceLoad까지) 다시 생각
		CurrentExperience = Experience;
	}

	StartExperienceLoad();
}

void ULSExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	check(LoadState == ELSExperienceLoadState::Unloaded);

	LoadState = ELSExperienceLoadState::Loading;

	ULSAssetManager& AssetManager = ULSAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;

	// 이미 앞서, ServerSetCurrentExperience에서 우리는 ExperienceId를 넘겨주었는데, 여기서 CDO를 활용하여, GetPrimaryAssetId를 로딩할 대상으로 넣는다.
	// 왜 이렇게??
	// - GetPrimaryAssetId를 좀 더 자세히 보자
	// - GetPrimaryAssetId를 살펴봄으로써, 아래의 두가지를 알 수 있다.
	// 1. 우리는 B_LSDefaultExperience를 BP로 만든 이유
	// 2. CDO를 가져와서, GetPrimaryAssetId를 호출한 이유

	// 우리는 앞서 이미 CDO로 로딩하여, CDO를 사용하지 않고 CDO를 사용하여 로딩할 에셋을 지정하여, BundleAssetList에 준다.
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	// load assets associated with the experience
	// 아래는 우리가 후일 GameFeature를 사용하여, Experience에 바인딩 된, GameFeature Plaugin을 로딩할 Bundle이름을 추가하한다.
	// - Bundle 이라는게 후일 우리가 로딩할 에셋의 카테로기 이름이라고 생각하면 된다
	TArray<FName> BundlesToLoad;
	{
		// 여기서 주목해야 할 부분은 OwnerNetMode가 NM_Standalone이면? Client/Server 둘다 로딩에 추가된다.
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
	// 아래도, 후일 Bundle을 우리가 GameFeature에 연동하면서 더 깊게 알아보자, 지금은 B_LSDefaultExperience를 로딩해주는 함수로 생각
	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundlesToLoad, {}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);

	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		// 로딩이 완료되었으면, ExecuteDelegate를 통해 OnAssetsLoadedDelegate를 호출하자
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

	// FrameNumber를 주목해서 보자
	static int32 StartExperienceLoad_FrameNumber = GFrameNumber;
}
PRAGMA_ENABLE_OPTIMIZATION

void ULSExperienceManagerComponent::OnExperienceLoadComplete()
{
	// FrameNumber를 주목해서 보자
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	// 해당 함수가 불리는 것은 앞서 보았던 StreamableDelegateDelayHelper에 의해 불림
	OnExperienceFullLoadCompleted();
}

void ULSExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != ELSExperienceLoadState::Loaded);

	LoadState = ELSExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

const ULSExperienceDefinition* ULSExperienceManagerComponent::GetCurrentExperienceChecked()
{
	check(LoadState == ELSExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}
