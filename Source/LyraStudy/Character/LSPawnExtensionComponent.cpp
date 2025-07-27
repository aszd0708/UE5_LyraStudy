// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LSPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "LSLogChannels.h"
#include "LSGameplayTags.h"

/* feature Name �� component �����ϱ� component�� ����, pawn extension�� ���� ���� Ȯ�� �� �� �ִ�. */
const FName ULSPawnExtensionComponent::NAME_ActorFeatureName("PawnExtention");

ULSPawnExtensionComponent::ULSPawnExtensionComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void ULSPawnExtensionComponent::SetPawnData(const ULSPawnData* InPawnData)
{
	// Pawn�� ���� Authority�� ���� ���, SetPawnData�� �������� ����
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	// PawnData ������Ʈ
	PawnData = InPawnData;
}

void ULSPawnExtensionComponent::SetupPlayerInputComponent()
{
	// ForceUpdate�� �ٽ� InitState ���¸�ȯ ����
	CheckDefaultInitialization();
}

void ULSPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	// �ùٸ� Actor�� ��ϵǾ����� Ȯ��.
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(LogLS, Error, TEXT("this component has been added to a BP whose base class is not a Pawn!"));
		}
	}

	// GameFrameworkComponentManager �� InitState ����� ���� ��� ����:
	// - ����� ��ӹ޾Ҵ� IGameFrameworkInitStateInterface �޼��� RegisterInitStateFeature()�� Ȱ��
	// - �ش� �Լ��� ������ ����
	RegisterInitStateFeature();

	// ������� ���� �Լ�
	UGameFrameworkComponentManager* Manager = UGameFrameworkComponentManager::GetForActor(GetOwningActor());
}

void ULSPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// FeatureName�� NAME_None�� ������, Actor�� ��ϵ� ��� Feature Component�� InitState�� �����ϰڴٴ� �ǹ�
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	/*
	* InitState_Spawned�� ���� ��ȯ
	* -TryToChangeInitState�� �Ʒ��� ���� ����
	* 1. CanChangeInitState�� ���� ��ȯ ���ɼ� ���� �Ǵ�
	* 2. HandleChangeInitState�� ���� ���� ���� (Feature Component)
	* 3. BindOnActorInitStateChanged�� Bind�� Delegate�� ���ǿ� �°� ȣ��
	*    - LSPawnExtensionComponent�� ��� ��� Actor �� Feature ���� ��ȭ�� ���� OnActorInitStateChanged() �� ȣ���
	*/
	ensure(TryToChangeInitState(FLSGameplayTags::Get().InitState_Spawned));
	
	/*
	* �ش� �Լ��� �������̵� �Ѵ�
	* - �� �Լ��� ForceUpdateInitState�� ���� �������� �������ָ� �ȴ�
	* - ���� ���� ������Ʈ�� ���� (���� CanChangedInitState�� HandleChangeInitState�� ��������)
	*/ 
	CheckDefaultInitialization();
}

void ULSPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// �ռ�, OnRegister�� RegisterInitStateFeature()�� ���� ��������
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void ULSPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		// LSPawnExtenstionComponent�� �ٸ� Feature Component���� ���°� DataVailable�� �����Ͽ�, Sync�� ���ߴ� ������ �־��� (CanChangeInitState)
		// - �̸� �����ϰ� �ϱ� ����, OnActorInitStateChanged������ DataAvailable�� ���� ���������� CheckDefaultInitialization�� ȣ���Ͽ�, ���¸� Ȯ���Ѵ�
		const FLSGameplayTags& InitTags = FLSGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

bool ULSPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FLSGameplayTags& InitTags = FLSGameplayTags::Get();

	// InitState_Spawned �ʱ�ȭ
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
		// �Ƹ� PawnData�� ������ �����ϴ� ����̴�.
		if (!PawnData)
		{
			return false;
		}

		// LocallyControlled �� Pawn�� Controller �� ������ ����
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
		// Actor�� ���ε� �� ��� Featrue ���� DataAvailabel �����϶�, DataInitialized�� �Ѿ
		// - HaveAllFeaturesReachedInitState Ȯ��
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	// ���� �������� transition�� �ƴϸ� false;
	return false;
}

void ULSPawnExtensionComponent::CheckDefaultInitialization()
{
	// PawnExtensionComponent�� Feature Component���� �ʱ�ȭ�� �����ϴ� Component��.
	// - ���� Actor�� ���ε� �� Feature Component�鿡 ���� CheckDefaultInitialization�� ȣ���� �ֵ��� �Ѵ� (Force Update ���� ����)
	// - �� �޼��带 IGameFrameworkInitStateInterface�� �����ϴµ�, CheckDefaultInitializationForImplementers�̴�.
	// - ������ CheckDefaultInitializationForImplementers �� ����
	CheckDefaultInitializationForImplementers();

	const FLSGameplayTags& InitTags = FLSGameplayTags::Get();

	// ����� ���� InitState�� ���� �Ѱ���� �Ѵ�.
	static const TArray<FGameplayTag> StateChain = { InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady };

	// CanChangeInitState()�� HandleChangeInitState() �׸��� ChangeFeatureInitState ȣ���� ���� OnActorInitStateChanged Delegate ȣ����� ����
	// �Ʒ��� �ڵ带 �ѹ� ������
	// - �ش� �ڵ���� ���� ���� �����ϴ�
	//	- ��� ���¸� �����Ҷ� ���� �ѹ� ������Ʈ�� �Ѵ�
	//	- InitState�� ���� ��ȭ�� �������̴�. 
	//	 - ������Ʈ�� ���߸� ������ ��������� �Ѵ�.
	ContinueInitStateChain(StateChain);
}
