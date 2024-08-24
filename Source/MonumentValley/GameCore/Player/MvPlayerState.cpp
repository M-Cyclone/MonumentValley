// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Player/MvPlayerState.h"

#include "GameCore/Player/MvBrickControlComponent.h"
#include "GameCore/Player/MvCharacterControlComponent.h"

AMvPlayerState::AMvPlayerState() : Super()
{
    PrimaryActorTick.bCanEverTick = true;

    CharacterControlComponent = CreateDefaultSubobject<UMvCharacterControlComponent>(TEXT("Character Control Comp"));
    BrickControlComponent     = CreateDefaultSubobject<UMvBrickControlComponent>(TEXT("Brick Control Comp"));

    check(CharacterControlComponent);
    check(BrickControlComponent);
}

void AMvPlayerState::Possess(IMvControllableBrick* Brick)
{
    BrickControlComponent->Possess(Brick);
}

void AMvPlayerState::UnPossess()
{
    BrickControlComponent->UnPossess();
}
