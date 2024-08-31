// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/AI/MvAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GameCore/Player/MvPlayerAvatar.h"

void AMvAIController::BeginPlay()
{
    Super::BeginPlay();

    RunBehaviorTree(BehaviorTree);

    if (AMvPlayerAvatar* Player = Cast<AMvPlayerAvatar>(UGameplayStatics::GetPlayerPawn(this, 0)))
    {
        GetBlackboardComponent()->SetValueAsObject("PlayerAvatar", Player);
    }
}
