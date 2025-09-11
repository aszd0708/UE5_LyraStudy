# pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "LSCharacterPartTypes.h"
#include "LSControllerComponent_CharacterParts.generated.h"

USTRUCT(BlueprintType)
struct FLSControllerCharacterPartEntry
{
    GENERATED_BODY()

    /* Chracter Part에 대한 정의(메타데이터 == MetaData) */
    UPROPERTY(EditAnywhere)
    FLSCharacterPart Part;

    /* Character Part 핸들 (고유값) - Controller가 Possess하고 있는 Pawn에서 생성한(인스턴스) Character Part 핸들값 */
    FLSCharacterPartHandle Handle;
};

UCLASS(meta = (BlueprintSpawnableComponent))
class ULSControllerComponent_CharacterParts : public UControllerComponent
{
    GENERATED_BODY()
public:
    ULSControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(EditAnywhere, Category = Cosmeitcs)
    TArray<FLSControllerCharacterPartEntry> CharacterParts;
};