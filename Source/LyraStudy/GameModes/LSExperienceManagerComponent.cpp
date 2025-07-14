// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LSExperienceManagerComponent.h"

void ULSExperienceManagerComponent::CallOrRegister_OnExperiencedLoaded(FOnLSExperienceLoaded::FDelegate&& Delegate)
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
