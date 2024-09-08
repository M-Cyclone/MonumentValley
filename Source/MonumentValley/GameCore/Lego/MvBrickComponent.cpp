// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Lego/MvBrickComponent.h"

#include "GameCore/Lego/MvBrick.h"

// Sets default values
UMvBrickComponent::UMvBrickComponent()
{}

// Called when the game starts or when spawned
void UMvBrickComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool UMvBrickComponent::AddBrick(AMvBrick* NewBrick)
{
    if (!NewBrick)
    {
        return false;
    }

    for (const AMvBrick* Brick : Bricks)
    {
        if (Brick == NewBrick)
        {
            return false;
        }
    }

    Bricks.Push(NewBrick);
    return true;
}
