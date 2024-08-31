// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BehaviorTree/BTService.h"

#include "BTService_GetTargetLocation.generated.h"

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API UBTService_GetTargetLocation : public UBTService
{
    GENERATED_BODY()

public:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
