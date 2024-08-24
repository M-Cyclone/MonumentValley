// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Player/MvPlayerAvatarController.h"

#include "GameCore/Player/MvPlayerState.h"

void AMvPlayerAvatarController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
    Super::PreProcessInput(DeltaTime, bGamePaused);

    bTriggeredSetControlBrick = false;
    bTriggeredSetMoveTarget   = false;
}

void AMvPlayerAvatarController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
    Super::PostProcessInput(DeltaTime, bGamePaused);
}

AMvPlayerState* AMvPlayerAvatarController::GetMvPlayerState() const
{
    return CastChecked<AMvPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}
