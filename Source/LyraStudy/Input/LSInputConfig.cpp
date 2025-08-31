// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/LSInputConfig.h"
#include "LSLogChannels.h"

ULSInputConfig::ULSInputConfig(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UInputAction* ULSInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// NativeInputActions�� ��ȸ�ϸ�, Input���� ���� InputTag�� �ִ��� üũ
	// - ������, �׿� ���� InputActiond�� ��ȯ������, ���ٸ�, �׳� nullptr�� ��ȯ�Ѵ�.
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
	// AbilityInputActions�� ��ȸ�ϸ�, Input���� ���� InputTag�� �ִ��� üũ
	// - ������, �׿� ���� InputActiond�� ��ȯ������, ���ٸ�, �׳� nullptr�� ��ȯ�Ѵ�.
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
