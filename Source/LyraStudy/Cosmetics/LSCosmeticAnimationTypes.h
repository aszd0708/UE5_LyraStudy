// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LSCosmeticAnimationTypes.generated.h"

USTRUCT(BlueprintType)
struct FLSAnimBodyStyleSelectionEntry
{
	GENERATED_BODY()

	/* AnimLayer를 적용할 대상 SkeletalMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> Mesh = nullptr;

	/* Cosmetic Tag라고 생각하면 됨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Cosmetic"))
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FLSAnimBodyStyleSelectionSet
{
	GENERATED_BODY()

	/* AnimLayer 적용할 SkeletalMEsh를 들고있음 -> ANimation-Mesh간 Rules을 MeshRules라고 생각하면 됨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLSAnimBodyStyleSelectionEntry> MeshRules;

	/* 그냥 디폴트로 적용할 SkeletalMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;

	/* Physics Asset은 하나로 동일함 -> 즉 모든 Animation의 Physics 속성은 공유함 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicsAsset> ForcePhysicsAsset = nullptr;
};

