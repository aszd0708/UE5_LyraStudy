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

/* FeatureName ����  : static member vaiable �ʱ�ȭ */
const FName ULSHeroComponent::NAME_ActorFeatureName("Hero");

ULSHeroComponent::ULSHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void ULSHeroComponent::OnRegister()
{
	Super::OnRegister();

	// �ùٸ� Actor�� ��ϵǾ����� Ȯ��
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

	// PawnExtensionComponent�� ���ؼ� (PawnExtension Feature) OnActorInitStateChanged() �����ϵ��� (Observing)
	BindOnActorInitStateChanged(ULSPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);
	
	// InitState_Spawned�� �ʱ�ȭ
	ensure(TryToChangeInitState(FLSGameplayTags::Get().InitState_Spawned));

	// ForceUpdate ����
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
		// LSPawnExtensionComponent�� DataInitialized ���� ��ȭ ���� ��, LSHeroComponent�� DataInitialized���·� ����
		// - CanChangeInitState Ȯ��
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

	// Spawned �ʱ�ȭ
	if (!CurrentState.IsValid() && DesiredState == InitTags.InitState_Spawned)
	{
		// Pawn�� �� ���ø� �Ǿ������� �ٷ� Spawned�� �Ѿ
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
		// PawnExtensionComponent�� DataInitialized �� ������ ��ٸ� (== ��� Feature Component�� DataAvailable�� ����)
		return Manager->HasFeatureReachedInitState(Pawn, ULSPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	// ���� �������� transition�� �ƴϸ� false;
	return false;
}

void ULSHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	const FLSGameplayTags& InitTags = FLSGameplayTags::Get();

	// DataAvailable -> DataInitialized �ܰ�
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ALSPlayerState* LSPS = GetPlayerState<ALSPlayerState>();
		if (!ensure(Pawn && LSPS))
		{
			return;
		}

		// Input�� Camera�� ���� �ڵ鸵 (TODO)

		const bool bIsLocallyController = Pawn->IsLocallyControlled();
		const ULSPawnData* PawnData = nullptr;
		if (ULSPawnExtensionComponent* PawnExtComp = ULSPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<ULSPawnData>();
		}

		if (bIsLocallyController && PawnData)
		{
			// ���� LSCharacter�� Attach�� CameraComponent�� ã��
			if (ULSCameraComponent* CameraComponent = ULSCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DeterminCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}

void ULSHeroComponent::CheckDefaultInitialization()
{
	// �ռ� BindOnActorInitStateChanged���� ���ҵ��� Hero Feature�� Pawn Extension Feature�� ���ӵǾ� �����Ƿ�, CheckDefaultInitializationForImplementers ȣ������ ����.

	// ContinueInitStateChain�� �ռ� PawnExtComponent�� ����
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