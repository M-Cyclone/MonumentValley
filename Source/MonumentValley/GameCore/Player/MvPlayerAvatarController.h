// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"

#include "MvPlayerAvatarController.generated.h"

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API AMvPlayerAvatarController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
    virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

public:

public:
    void TriggeredSetMoveTarget() { bTriggeredSetMoveTarget = true; }
    void TriggeredSetControlBrick() { bTriggeredSetControlBrick = true; }

    bool IsTriggeredSetMoveTarget() const { return bTriggeredSetMoveTarget; }
    bool IsTriggeredSetControlBrick() const { return bTriggeredSetControlBrick; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monument Valley|Input")
    bool bTriggeredSetMoveTarget = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monument Valley|Input")
    bool bTriggeredSetControlBrick = false;
};
