// Fill out your copyright notice in the Description page of Project Settings.


#include "Cosmetics/LSCosmeticAnimationTypes.h"

USkeletalMesh* FLSAnimBodyStyleSelectionSet::SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const
{
	// MeshRule을 순회하며, CosmeticTags요구 조건에 맞는 MeshRule을 찾아 SkeletalMesh를 반환한다.
	for (const FLSAnimBodyStyleSelectionEntry& Rule : MeshRules)
	{
		if (Rule.Mesh && CosmeticTags.HasAll(Rule.RequiredTags))
		{
			return Rule.Mesh;
		}
	}
	return DefaultMesh;
}
