// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LSPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "LSLogChannels.h"
#include "LSGameplayTags.h"

/* feature Name 을 component 단위니깐 component를 빼고, pawn extension만 넣을 것을 확인 할 수 있다. */
const FName ULSPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

ULSPawnExtensionComponent::ULSPawnExtensionComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void ULSPawnExtensionComponent::SetPawnData(const ULSPawnData* InPawnData)
{
	// Pawn에 대해 Authority가 없을 경우, SetPawnData는 진행하지 않음
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	// PawnData 업데이트
	PawnData = InPawnData;
}

void ULSPawnExtensionComponent::SetupPlayerInputComponent()
{
	// ForceUpdate로 다시 InitState 상태면환 시작
	CheckDefaultInitialization();
}

void ULSPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	// 올바른 Actor에 등록되었는지 확인.
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(LogLS, Error, TEXT("this component has been added to a BP whose base class is not a Pawn!"));
		}
	}

	// GameFrameworkComponentManager 에 InitState 사용을 위해 등록 진행:
	// - 등록은 상속받았던 IGameFrameworkInitStateInterface 메서드 RegisterInitStateFeature()를 활용
	// - 해당 함수를 간단히 보기
	RegisterInitStateFeature();

	// 디버깅을 위한 함수
	UGameFrameworkComponentManager* Manager = UGameFrameworkComponentManager::GetForActor(GetOwningActor());
}

void ULSPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// FeatureName에 NAME_None을 넣으면, Actor에 등록된 모든 Feature Component의 InitState를 관찰하겠다는 의미
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	/*
	* InitState_Spawned로 상태 변환
	* -TryToChangeInitState는 아래와 같이 진행
	* 1. CanChangeInitState로 상태 변환 가능성 유무 판단
	* 2. HandleChangeInitState로 내부 상태 변경 (Feature Component)
	* 3. BindOnActorInitStateChanged로 Bind된 Delegate를 조건에 맞게 호출
	*    - LSPawnExtensionComponent의 경우 모든 Actor 의 Feature 상태 변화에 대해 OnActorInitStateChanged() 가 호출됨
	*/
	ensure(TryToChangeInitState(FLSGameplayTags::Get().InitState_Spawned));
	
	/*
	* 해당 함수를 오버라이딩 한다
	* - 이 함수를 ForceUpdateInitState와 같은 느낌으로 이해해주면 된다
	* - 현재 강제 업데이트를 진행 (물론 CanChangedInitState와 HandleChangeInitState를 진행해줌)
	*/ 
	CheckDefaultInitialization();
}

void ULSPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 앞서, OnRegister의 RegisterInitStateFeature()의 쌍을 맞춰주자
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void ULSPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	UE_LOG(LogLS, Error, TEXT("ULSPawnExtensionComponent::OnActorInitStateChanged FeatureState : %s"), *Params.FeatureState.ToString());
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		// LSPawnExtenstionComponent는 다른 Feature Component들의 상태가 DataVailable을 관찰하여, Sync를 맞추는 구간이 있었다 (CanChangeInitState)
		// - 이를 가능하게 하기 위해, OnActorInitStateChanged에서는 DataAvailable에 대해 지속적으로 CheckDefaultInitialization을 호출하여, 상태를 확인한다
		const FLSGameplayTags& InitTags = FLSGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

bool ULSPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	UE_LOG(LogLS, Error, TEXT("====================================================================================="));
	UE_LOG(LogLS, Error, TEXT("ULSPawnExtensionComponent::CanChangeInitState CurrentState : %s"), *CurrentState.ToString());
	UE_LOG(LogLS, Error, TEXT("ULSPawnExtensionComponent::CanChangeInitState DesiredState : %s"), *DesiredState.ToString());
	UE_LOG(LogLS, Error, TEXT("====================================================================================="));
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FLSGameplayTags& InitTags = FLSGameplayTags::Get();

	// InitState_Spawned 초기화
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// Pawn이 잘 세팅만 되어있으면 바로 Spawned로 넘어감
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		// 아마 PawnData를 누군가 설정하는 모양이다.
		if (!PawnData)
		{
			UE_LOG(LogLS, Error, TEXT("PawnData Is null"));
			return false;
		}

		// LocallyControlled 인 Pawn이 Controller 가 없으면 에러
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		if (bIsLocallyControlled)
		{
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// Actor에 바인드 된 모든 Featrue 들이 DataAvailabel 상태일때, DataInitialized로 넘어감
		// - HaveAllFeaturesReachedInitState 확인
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		UE_LOG(LogLS, Error, TEXT("ULSPawnExtensionComponent::CanChangeInitState Is End"));
		return true;
	}

	// 위의 선형적인 transition이 아니면 false;
	return false;
}

void ULSPawnExtensionComponent::CheckDefaultInitialization()
{
	// PawnExtensionComponent는 Feature Component들의 초기화를 관장하는 Component임.
	// - 따라서 Actor에 바인딘 된 Feature Component들에 대해 CheckDefaultInitialization을 호출해 주도록 한다 (Force Update 같은 느낌)
	// - 이 메서드를 IGameFrameworkInitStateInterface가 제공하는데, CheckDefaultInitializationForImplementers이다.
	// - 간단히 CheckDefaultInitializationForImplementers 를 보자
	CheckDefaultInitializationForImplementers();

	const FLSGameplayTags& InitTags = FLSGameplayTags::Get();

	// 사용자 정의 InitState를 직접 넘겨줘야 한다.
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	// CanChangeInitState()와 HandleChangeInitState() 그리고 ChangeFeatureInitState 호출을 통한 OnActorInitStateChanged Delegate 호출까지 진행
	// 아래의 코드를 한번 봐보자
	// - 해당 코드까지 보면 짐작 가능하다
	//	- 계속 상태를 가능할때 까지 한번 업데이트를 한다
	//	- InitState에 대한 변화는 선형적이다. 
	//	 - 업데이트가 멈추면 누군가 시작해줘야 한다.
	ContinueInitStateChain(StateChain);
}
