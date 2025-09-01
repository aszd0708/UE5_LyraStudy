// Fill out your copyright notice in the Description page of Project Settings.


#include "LSUserFacingExperience.h"
#include "CommonSessionSubsystem.h"

UCommonSession_HostSessionRequest* ULSUserFacingExperience::CreateHostRequest() const
{
	const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();

	/*
	* - UCommonSession_HostSessionRequest�� UObject�� �����س���, �˾Ƽ� GC�� �ȴ�.
	* - �ش� ��ü�� ���� �����ӿ��� ����ϱ� ������, GC�� ���� ������ �ʿ����: ���� ���� �������̵� �߰����� ������ �󿡼� �ش� ��ü�� ����� ���, LiveTime ���� �ʿ�
	* - �׷��� ������ dangling ����.
	*/

	UCommonSession_HostSessionRequest* Result = NewObject< UCommonSession_HostSessionRequest>();
	Result->MapID = MapID;
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);

	return Result;
}
