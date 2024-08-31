// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AIController.h"

#include "MvAIController.generated.h"

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API AMvAIController : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Monument Valley|AI")
    TObjectPtr<UBehaviorTree> BehaviorTree;
};
