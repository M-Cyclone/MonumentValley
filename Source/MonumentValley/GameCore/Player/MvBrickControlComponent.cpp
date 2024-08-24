// Fill out your copyright notice in the Description page of Project Settings.


#include "MvBrickControlComponent.h"

#include "GameCore/Player/MvControllableBrick.h"

// Sets default values for this component's properties
UMvBrickControlComponent::UMvBrickControlComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

// Called when the game starts
void UMvBrickControlComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...
}

// Called every frame
void UMvBrickControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void UMvBrickControlComponent::Possess(IMvControllableBrick* Brick)
{
    if (PossessingBrick == Brick)
    {
        return;
    }

    UnPossess();
    if (Brick)
    {
        PossessingBrick = Brick;
        PossessingBrick->PossessedBy(this);
    }
}

void UMvBrickControlComponent::UnPossess()
{
    if (PossessingBrick)
    {
        PossessingBrick->UnPossessed();
        PossessingBrick = nullptr;
    }
}
