// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Player/MvPlayerAvatarController.h"

void AMvPlayerAvatarController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
    Super::PreProcessInput(DeltaTime, bGamePaused);

    bTriggeredSetControlBrick = false;
    bTriggeredSetMoveTarget   = false;
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AMvPlayerAvatarController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
    Super::PostProcessInput(DeltaTime, bGamePaused);
}
