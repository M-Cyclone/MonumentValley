// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MvBrickControlComponent.h"

#include "GameFramework/PlayerState.h"

#include "MvPlayerState.generated.h"

class IMvControllableBrick;
class UMvBrickControlComponent;
class UMvCharacterControlComponent;

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API AMvPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AMvPlayerState();

public:
    void Possess(IMvControllableBrick* Brick);
    void UnPossess();
    bool IsPossessingBrick() const { return BrickControlComponent->IsPossessingBrick(); }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monument Valley|Control")
    TObjectPtr<UMvCharacterControlComponent> CharacterControlComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monument Valley|Control")
    TObjectPtr<UMvBrickControlComponent> BrickControlComponent;
};
