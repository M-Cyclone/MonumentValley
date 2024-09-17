// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/MovementComponent.h"

#include "MvMovementComponent.generated.h"

class AMvBrickMap;
class UMvBrickComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MONUMENTVALLEY_API UMvMovementComponent : public UMovementComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UMvMovementComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    void SetBrickComponent(UMvBrickComponent* NewComp) { BrickComp = NewComp; }

protected:
    UPROPERTY()
    TObjectPtr<UMvBrickComponent> BrickComp;

public:
    void OnSetCurrMap(UMvBrickComponent* Comp);

protected:
    UPROPERTY()
    FVector2f MapSpaceLoc2D = FVector2f::Zero();

    UPROPERTY()
    FIntVector TargetBrick = FIntVector(-1);

    UPROPERTY()
    TArray<FIntVector> PathToTarget;

    UPROPERTY()
    int32 CurrPathIndex = -1;

    // Movement params.
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monument Valley|Movement")
    float MoveVelocity = 3.0f;  // About 3m/s
};
