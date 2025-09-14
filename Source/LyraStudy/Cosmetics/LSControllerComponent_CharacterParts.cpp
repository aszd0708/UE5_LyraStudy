#include "LSControllerComponent_CharacterParts.h"
#include "LSPawnComponent_CharacterParts.h"

ULSControllerComponent_CharacterParts::ULSControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void ULSControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (AController* OwningController = GetController<AController>())
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
		}
	}
}

void ULSControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveCharacterParts();

	Super::EndPlay(EndPlayReason);
}

ULSPawnComponent_CharacterParts* ULSControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// 생각해보면, 우리는 앞어 LSPawnComponent_CharacterParts를 상속받는 B_MannequinPawnCosmetics를 이미 B_Hero_ShooterMannequin에 추가했음
		// B_MannequinPawnCosmetics를 반환되길 기대한다
		return ControlledPawn->FindComponentByClass<ULSPawnComponent_CharacterParts>();
	}
	return nullptr;
}

void ULSControllerComponent_CharacterParts::AddCharacterPart(const FLSCharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart);
}

void ULSControllerComponent_CharacterParts::AddCharacterPartInternal(const FLSCharacterPart& NewPart)
{
	FLSControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;

	if (ULSPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
	}
}

void ULSControllerComponent_CharacterParts::RemoveCharacterParts()
{
	if (ULSPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (FLSControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}
	CharacterParts.Reset();
}

void ULSControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// 이전 OldPawn에 대해서는 Character Parts를 전부 제거해주자
	if (ULSPawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<ULSPawnComponent_CharacterParts>() : nullptr)
	{
		for (FLSControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	// 새로운 Pawn에 대해서 기존 Controller가 가지고 있는 Character Parts를 추가해주자
	if (ULSPawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<ULSPawnComponent_CharacterParts>() : nullptr)
	{
		for (FLSControllerCharacterPartEntry& Entry : CharacterParts)
		{
			check(!Entry.Handle.IsValid());
			Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
		}
	}
}
