// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MvControllableBrick.h"

#include "Components/ActorComponent.h"

#include "MvBrickControlComponent.generated.h"

class IMvControllableBrick;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MONUMENTVALLEY_API UMvBrickControlComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UMvBrickControlComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    void Possess(IMvControllableBrick* Brick);
    void UnPossess();
    bool IsPossessingBrick() const { return PossessingBrick != nullptr; }

protected:
    IMvControllableBrick* PossessingBrick = nullptr;
};
