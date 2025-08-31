// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LSHeroComponent.h"
#include "UObject/UObjectGlobals.h"
#include "LSLogChannels.h"
#include "PlayerMappableInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "Input/LSInputComponent.h"
#include "Input/LSMappableConfigPair.h"
#include "Character/LSPawnExtensionComponent.h"
#include "Character/LSPawnData.h"
#include "LSGameplayTags.h"
#include "Player/LSPlayerState.h"
#include "Player/LSPlayerController.h"
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
	UE_LOG(LogLS, Error, TEXT("ULSHeroComponent::OnActorInitStateChanged FeatureState : %s"), *Params.FeatureState.ToString());
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
		if (!Pawn)
		{
			UE_LOG(LogLS, Error, TEXT("Pawn Is Null"));
		}
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
		// PlayerState 유효성 검사를 HandleChangeInitState로 미룹니다.
		// PawnExtensionComponent가 초기화되었다는 것을 신뢰하고 상태 변경을 허용합니다.
		// PlayerState 리플리케이션 지연으로 인한 Race Condition을 해결하기 위함입니다.
		return LSPS && Manager->HasFeatureReachedInitState(Pawn, ULSPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
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
	UE_LOG(LogLS, Log, TEXT("ULSHeroComponent::HandleChangeInitState trying to transition from %s to %s"), *CurrentState.ToString(), *DesiredState.ToString());

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

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const ULSPawnData* PawnData = nullptr;
		if (ULSPawnExtensionComponent* PawnExtComp = ULSPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<ULSPawnData>();
		}

		if (bIsLocallyControlled && PawnData)
		{
			// 현재 HakCharacter에 Attach된 CameraComponent를 찾음
			if (ULSCameraComponent* CameraComponent = ULSCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}

		if (LSPS)
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
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
void ULSHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	// LocalPlayer을 가져오기 위함
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	// EnhancedInputLocalPlayerSubsystem 을 가져오기 위함
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	// EnhancedInputLocalPlayerSubsystem에 MappingContext를 비워준다.
	Subsystem->ClearAllMappings();

	// PawnExtensionComponent -> PawnData -> InputConfig 존재 유무 판단
	if (const ULSPawnExtensionComponent* PawnExtComp = ULSPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const ULSPawnData* PawnData = PawnExtComp->GetPawnData<ULSPawnData>())
		{
			if (const ULSInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FLSGameplayTags& GameplayTags = FLSGameplayTags::Get();

				// HeroComponent 가지고 있는 Input Mapping Context를 순회하며, EnhancedInputLocalPlayerSubsystem에 추가한다.
				for (const FLSMappableConfigPair& Pair : DefaultInputConfigs)
				{
					if (Pair.bShouldActivateAutomatically)
					{
						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;

						// 내부적으로 Input Mapping Context를 추가한다.
						// - AddPlayerMappableConfig를 간단히 보는 것을 추천
						Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
					}
				}

				ULSInputComponent* LSIC = CastChecked<ULSInputComponent>(PlayerInputComponent);
				{
					// InputTag_Move와 InputTag_Look_Mouse에 대해 각각 Input_Move()와 Input_LookMouse() 멤버 함수에 바인딩 시킨다.
					// - 바인딩한 이후, Input 이벤트에 따라 멤버 함수가 트리거 된다
					LSIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
					LSIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
				}
			}
		}
	}
}
void ULSHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			// Left/Right -> X 값에 들어있음.
			// MovementDirection은 현재 카메라의 RightVector를 의미함 (World-Space)
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);

			// AddMovementInput 함수를 한번 보자:
			// - 내부적으로 MovementDirection * Value.X 를 MovementCOmponent에 적용 해준다
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			// 앞서 Left/Right 와 마찬가지로 Forward/Backward를 적용한다.
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}
void ULSHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn) 
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		// X에는 Yaw값이 있음.
		// - Camera에 대해 Yaw 적용
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		// Y에는 Pitch값이 있음.
		double AnimInversionValue = -Value.Y;
		Pawn->AddControllerPitchInput(AnimInversionValue);
	}
}
PRAGMA_ENABLE_OPTIMIZATION
