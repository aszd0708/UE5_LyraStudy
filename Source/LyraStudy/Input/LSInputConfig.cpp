// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/LSInputConfig.h"
#include "LSLogChannels.h"

ULSInputConfig::ULSInputConfig(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UInputAction* ULSInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// NativeInputActions를 순회하며, Input으로 들어온 InputTag가 있는지 체크
	// - 있으면, 그에 따른 InputActiond르 반환하지만, 없다면, 그냥 nullptr을 반환한다.
	for (const FLSInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogLS, Error, TEXT("can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* ULSInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// AbilityInputActions를 순회하며, Input으로 들어온 InputTag가 있는지 체크
	// - 있으면, 그에 따른 InputActiond르 반환하지만, 없다면, 그냥 nullptr을 반환한다.
	for (const FLSInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogLS, Error, TEXT("can't find AbilityInputActions for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
