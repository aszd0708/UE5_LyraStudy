// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "LSPawnExtensionComponent.generated.h"

class ULSPawnData;
/**
 * �ʱ�ȭ ������ ����ϴ� ������Ʈ
 */
UCLASS()
class LYRASTUDY_API ULSPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	ULSPawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* member method
	*/
	static ULSPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) {return (Actor? Actor->FindComponentByClass<ULSPawnExtensionComponent>() : nullptr); }
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void SetPawnData(const ULSPawnData* InPawnData);
	void SetupPlayerInputComponent();

	/* FeatureName ���� */
	static const FName NAME_ActorFeatureName;

	/*
	* PawnComponent interface
	*/
	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	/*
	* IGameFrameworkInitStateInterface
	*/
	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void CheckDefaultInitialization();

	/*
	* Pawn �� ������ ������ ĳ��
	*/
	UPROPERTY(EditInstanceOnly, Category = "LS|Pawn")
	TObjectPtr<const ULSPawnData> PawnData;
};
