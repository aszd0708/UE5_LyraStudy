// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputTriggers.h"
#include "InputActionValue.h"
#include "LSInputConfig.h"
#include "EnhancedInputComponent.h"
#include "LSInputComponent.generated.h"

/**
 * 
 */

UCLASS()
class LYRASTUDY_API ULSInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	ULSInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* 
	* member methods
	*/
	template <class UserClass, typename FuncType>
	void BindNativeAction(const ULSInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const ULSInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);
};

template<class UserClass, typename FuncType>
inline void ULSInputComponent::BindNativeAction(const ULSInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);

	// ���⼭ �� �� �ֵ���, InputConfig�� Ȱ��ȭ �����ϴ� InputAction�� ����ִ�.
	// - ����, InputConfig�� ���� InputAction �� Binding��Ű��, nullptr�� ��ȯ�Ͽ�, ���ε� �ϴµ� �����Ѵ�.
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
inline void ULSInputComponent::BindAbilityActions(const ULSInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	// AbilityAction�� ���ؼ��� ��� InputAction �� ���� ���ε� �Ѵ�.
	for (const FLSInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
