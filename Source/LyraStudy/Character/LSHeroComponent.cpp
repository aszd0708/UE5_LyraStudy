// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LSHeroComponent.h"
#include "UObject/UObjectGlobals.h"
#include "LSLogChannels.h"
#include "Character/LSPawnExtensionComponent.h"
#include "Character/LSPawnData.h"
#include "LSGameplayTags.h"
#include "Player/LSPlayerState.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Camera/LSCameraComponent.h"

/* FeatureName 정의  : static member vaiable 초기화 */
const FName ULSHeroComponent::NAME_ActorFeatureName("Hero");

ULSHeroComponent::ULSHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void ULSHeroComponent::OnRegister()
{
	Super::OnRegister();

	// 올바른 Actor에 등록되었는지 확인
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(LogLS, Error, TEXT("this component has been added to a BP whose base class in not a pawn"));
		}
	}

	RegisterInitStateFeature();
}

void ULSHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// PawnExtensionComponent에 대해서 (PawnExtension Feature) OnActorInitStateChanged() 관찰하도록 (Observing)
	BindOnActorInitStateChanged(ULSPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);
	
	// InitState_Spawned로 초기화
	ensure(TryToChangeInitState(FLSGameplayTags::Get().InitState_Spawned));

	// ForceUpdate 진행
	CheckDefaultInitialization();
}

void ULSHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}


void ULSHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	const FLSGameplayTags& InitTags = FLSGameplayTags::Get();
	if (Params.FeatureName == ULSPawnExtensionComponent::NAME_ActorFeatureName)
	{
		// LSPawnExtensionComponent의 DataInitialized 상태 변화 관찰 후, LSHeroComponent도 DataInitialized상태로 변경
		// - CanChangeInitState 확인
		if (Params.FeatureState == InitTags.InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

bool ULSHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FLSGameplayTags& InitTags = FLSGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	ALSPlayerState* LSPS = GetPlayerState<ALSPlayerState>();

	// Spawned 초기화
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
		if (!LSPS)
		{
			return false;
		}

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// PawnExtensionComponent가 DataInitialized 될 때까지 기다림 (== 모든 Feature Component가 DataAvailable인 상태)
		return Manager->HasFeatureReachedInitState(Pawn, ULSPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	// 위의 선형적인 transition이 아니면 false;
	return false;
}

void ULSHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FLSGameplayTags& InitTags = FLSGameplayTags::Get();

	// DataAvailable -> DataInitialized 단계
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ALSPlayerState* LSPS = GetPlayerState<ALSPlayerState>();
		if (!ensure(Pawn && LSPS))
		{
			return;
		}

		// Input과 Camera에 대한 핸들링 (TODO)

		const bool bIsLocallyController = Pawn->IsLocallyControlled();
		const ULSPawnData* PawnData = nullptr;
		if (ULSPawnExtensionComponent* PawnExtComp = ULSPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<ULSPawnData>();
		}

		if (bIsLocallyController && PawnData)
		{
			// 현재 LSCharacter에 Attach된 CameraComponent를 찾음
			if (ULSCameraComponent* CameraComponent = ULSCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DeterminCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}

void ULSHeroComponent::CheckDefaultInitialization()
{
	// 앞서 BindOnActorInitStateChanged에서 보았듯이 Hero Feature는 Pawn Extension Feature에 종속되어 잇으므로, CheckDefaultInitializationForImplementers 호출하지 않음.

	// ContinueInitStateChain은 앞서 PawnExtComponent와 같음
	const FLSGameplayTags& InitTags = FLSGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

PRAGMA_DISABLE_OPTIMIZATION
TSubclassOf<ULSCameraMode> ULSHeroComponent::DetermineCameraMode() const
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (ULSPawnExtensionComponent* PawnExtComp = ULSPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const ULSPawnData* PawnData = PawnExtComp->GetPawnData<ULSPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}
	return nullptr;
}
PRAGMA_ENABLE_OPTIMIZATION