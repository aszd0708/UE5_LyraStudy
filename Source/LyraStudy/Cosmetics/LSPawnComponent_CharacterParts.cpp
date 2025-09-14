// Fill out your copyright notice in the Description page of Project Settings.


#include "Cosmetics/LSPawnComponent_CharacterParts.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Character.h"

ULSPawnComponent_CharacterParts::ULSPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), CharacterPartList(this)
{
}

USkeletalMeshComponent* ULSPawnComponent_CharacterParts::GetParentMeshComponent() const
{	
	// Character를 활용하여, 최상위 SkeletalMesh를 반환한다
	if (AActor* OwnerActor = GetOwner())
	{
		if (ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor))
		{
			if (USkeletalMeshComponent* MeshComponent = OwningCharacter->GetMesh())
			{
				return MeshComponent;
			}
		}
	}
	return nullptr;
}

USceneComponent* ULSPawnComponent_CharacterParts::GetSceneComponentToAttachTo() const
{
	// Parent에 SkeletalMeshComponent가 있으면 반환하고,
	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		return MeshComponent;
	}
	// 그리고 RootComponent 확인
	else if (AActor* OwnerActor = GetOwner())
	{
		return OwnerActor->GetRootComponent();
	}
	
	return nullptr;
}

FGameplayTagContainer ULSPawnComponent_CharacterParts::GetCombinedTags(FGameplayTag RequiredPrefix) const
{
	// 현재 장착된 CharacterPartList의 Merge된 Tags를 반환한다.
	FGameplayTagContainer Result = CharacterPartList.CollectCombinedTags();
	if (RequiredPrefix.IsValid())
	{
		// 만약 GameplayTag를 통해 필터리할 경우, 필터링해서 진행한다.
		return Result.Filter(FGameplayTagContainer(RequiredPrefix));
	}
	else
	{
		// 필터링 할 GameplayTag가 없으면 그냥 반환
		return Result;
	}
}

void ULSPawnComponent_CharacterParts::BroadcastChanged()
{
	const bool bReinitPose = true;

	// 현재 Owner의 SkeletalMeshComponent를 반환한다.
	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		// BodyMeshes를 통해, GameplayTag를 활용하여, 알맞은 SkeletalMesh로 ㄹ재설정해준다.
		const FGameplayTagContainer MergedTags = GetCombinedTags(FGameplayTag());
		USkeletalMesh* DesiredMesh = BodyMeshes.SelectBestBodyStyle(MergedTags);

		// SkeletalMesh를 초기화 및 ANimation초기화 시켜준다.
		MeshComponent->SetSkeletalMesh(DesiredMesh, bReinitPose);

		// PhysicsAsset을 초기화 해준다.
		if (UPhysicsAsset* PhysicsAsset = BodyMeshes.ForcePhysicsAsset)
		{
			MeshComponent->SetPhysicsAsset(PhysicsAsset, bReinitPose);
		}
	}
}

FLSCharacterPartHandle ULSPawnComponent_CharacterParts::AddCharacterPart(const FLSCharacterPart& NewPart)
{
	return CharacterPartList.AddEntry(NewPart);
}

void ULSPawnComponent_CharacterParts::RemoveCharacterPart(FLSCharacterPartHandle Handle)
{
	CharacterPartList.RemoveEntry(Handle);
}

bool FLSCharacterPartList::SpawnActorForEntry(FLSAppliedCharacterPartEntry& Entry)
{
	bool bCreatedAnyActor = false;
	// 전달된 AppliedCharacterPartEntry의 Part Class가 제대로 세팅되어 있다면
	if (Entry.Part.PartClass != nullptr)
	{
		// OwnerComponent의 Owner에 속한 World를 반환
		UWorld* World = OwnerComponent->GetWorld();

		// HakPawnComponent_CharacterParts에 어느 Component에 붙일 것인지 결정한다: 
		// - GetSceneComponentToAttachTo
		if (USceneComponent* ComponentToAttachTo = OwnerComponent->GetSceneComponentToAttachTo())
		{
			// 붙일 Component인 ComponentToAttachTo의 Bone 혹은 SocketName을 통해 어디에 붙일지 Transform을 계산한다
			const FTransform SpawnTransform = ComponentToAttachTo->GetSocketTransform(Entry.Part.SocketName);

			// 우리는 Actor-Actor의 결합이므로, ChildActorComponent를 활용한다
			UChildActorComponent* PartComponent = NewObject<UChildActorComponent>(OwnerComponent->GetOwner());
			PartComponent->SetupAttachment(ComponentToAttachTo, Entry.Part.SocketName);
			PartComponent->SetChildActorClass(Entry.Part.PartClass);
			// 참고로 RegisterComponent를 통해 마지막으로 RenderWorld인 FScene에 변경 내용을 전달한다 (혹은 생성한다)
			PartComponent->RegisterComponent();

			// ChildActorComponent에서 생성한 Actor를 반환하여
			if (AActor* SpawnedActor = PartComponent->GetChildActor())
			{
				// 해당 Actor가 Parent인 HakPawnComponent_CharacterParts의 Owner Actor보다 먼저 Tick이 실행되지 않도록 선행조건을 붙인다
				if (USceneComponent* SpawnedRootComponent = SpawnedActor->GetRootComponent())
				{
					SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
				}
			}

			Entry.SpawnedComponent = PartComponent;
			bCreatedAnyActor = true;
		}
	}

	return bCreatedAnyActor;
}

void FLSCharacterPartList::DestroyActorForEntry(FLSAppliedCharacterPartEntry& Entry)
{
	if (Entry.SpawnedComponent)
	{
		Entry.SpawnedComponent->DestroyComponent();
		Entry.SpawnedComponent = nullptr;
	}
}

FLSCharacterPartHandle FLSCharacterPartList::AddEntry(FLSCharacterPart NewPart)
{
	// PawnComponent의 CharacterPartList가 PartHandle를 관리하고, 이를 ControllerComponent_CharacterParts에 전달한다.
	FLSCharacterPartHandle Result;
	Result.PartHandle = PartHandleCounter++;

	// Authority가 있다면, AppliedCharacterPartEntry를 Entries에 추가한다.
	if (ensure(OwnerComponent && OwnerComponent->GetOwner() && OwnerComponent->GetOwner()->HasAuthority()))
	{
		FLSAppliedCharacterPartEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Part = NewPart;
		NewEntry.PartHandle = Result.PartHandle;

		// 여기서 실제 Actor를 생성하고, OwnerComponent의 Owner Actor에 Actor끼리 RootComponent로 Attach 시킨다
		if (SpawnActorForEntry(NewEntry))
		{
			// BroadcastChanged를 통해, OwnerComponent에서 Owner의 SkeletalMeshComponent를 활용하여, Aniumation 및 Phusics를 Re-Iniitalized 한다.
			OwnerComponent->BroadcastChanged();
		}
	}

	return Result;
}

void FLSCharacterPartList::RemoveEntry(FLSCharacterPartHandle Handle)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FLSAppliedCharacterPartEntry& Entry = *EntryIt;

		// 제거할 경우, PartHandle을 활용한다
		if (Entry.PartHandle == Handle.PartHandle)
		{
			DestroyActorForEntry(Entry);
		}
	}
}

FGameplayTagContainer FLSCharacterPartList::CollectCombinedTags() const
{
	FGameplayTagContainer Result;

	// Entries를 순회하며,
	for (const FLSAppliedCharacterPartEntry& Entry : Entries)
	{
		// Part Actor 가 생성되어 SpawnedComponent에 캐싱 되어있으면?
		if (Entry.SpawnedComponent)
		{
			// 해당 Actor의 IGameplayTagAssetInterface를 통해 GameplayTag를 검색한다.
			// - 현재 우리의 TaggedActor는 IGameplayTagAssetInterface를 상속받지 않으므로 그냥 넘어갈 것이다.
			// - 후일 여러분들이 각 Part에 대해 GameplayTag를 넣고 싶다면, 이걸 상속받아 정의해야 한다.:
			//		- 예를 들어, 특정 LV100 이상 장착 가능한 장비를 만들고 싶다면 넣어야 한다.
			if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Entry.SpawnedComponent->GetChildActor()))
			{
				TagInterface->GetOwnedGameplayTags(Result);
			}
		}
	}
	return Result;
}
