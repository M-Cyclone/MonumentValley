// Fill out your copyright notice in the Description page of Project Settings.


#include "MonumentValley/GameCore/System/MvGameModeBase.h"

#include "MonumentValley/GameCore/Player/MvPlayerController.h"
#include "MonumentValley/GameCore/Player/MvPlayerState.h"
#include "MvGameState.h"

AMvGameModeBase::AMvGameModeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    GameStateClass        = AMvGameState::StaticClass();
    PlayerControllerClass = AMvPlayerController::StaticClass();
    PlayerStateClass      = AMvPlayerState::StaticClass();
}

void AMvGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
}

UClass* AMvGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AMvGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
    return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
}

bool AMvGameModeBase::ShouldSpawnAtStartSpot(AController* Player)
{
    return Super::ShouldSpawnAtStartSpot(Player);
}

void AMvGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

AActor* AMvGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
    return Super::ChoosePlayerStart_Implementation(Player);
}

void AMvGameModeBase::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
    Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

bool AMvGameModeBase::PlayerCanRestart_Implementation(APlayerController* Player)
{
    return Super::PlayerCanRestart_Implementation(Player);
}

void AMvGameModeBase::InitGameState()
{
    Super::InitGameState();
}

bool AMvGameModeBase::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
    return Super::UpdatePlayerStartSpot(Player, Portal, OutErrorMessage);
}

void AMvGameModeBase::GenericPlayerInitialization(AController* NewPlayer)
{
    Super::GenericPlayerInitialization(NewPlayer);
}

void AMvGameModeBase::FailedToRestartPlayer(AController* NewPlayer)
{
    Super::FailedToRestartPlayer(NewPlayer);
}
