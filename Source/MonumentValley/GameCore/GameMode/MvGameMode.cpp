// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/GameMode/MvGameMode.h"

void AMvGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PC = Cast<APlayerController>(*Iterator);
        PC->bShowMouseCursor  = true;
    }
}