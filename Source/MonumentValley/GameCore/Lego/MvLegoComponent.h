// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "MvLegoComponent.generated.h"

struct FMouseInteractResult;

class AMvBrick;

USTRUCT(BlueprintType)
struct FLegoBrick
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<AMvBrick>> Bricks;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MONUMENTVALLEY_API UMvLegoComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UMvLegoComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    void ProcessControlBrick(const FMouseInteractResult& Input);
    void ProcessSetTargetPos(const FMouseInteractResult& Input);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FLegoBrick> LegoBricks;
};
