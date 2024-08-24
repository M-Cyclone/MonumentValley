// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"

#include "MvControllableBrick.generated.h"

class UMvBrickControlComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMvControllableBrick : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class MONUMENTVALLEY_API IMvControllableBrick
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    UFUNCTION(BlueprintNativeEvent)
    void PossessedBy(UMvBrickControlComponent* PlayerState);

    UFUNCTION(BlueprintNativeEvent)
    void UnPossessed();
};
