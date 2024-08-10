// Fill out your copyright notice in the Description page of Project Settings.


#include "MonumentValley/GameCore/System/MvGameState.h"

#include "GameFramework/PlayerState.h"

AMvGameState::AMvGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick          = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UMvAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AMvGameState::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AMvGameState::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    check(AbilitySystemComponent);
    AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/this, /*Avatar=*/this);
}

void AMvGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AMvGameState::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AMvGameState::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);
}

void AMvGameState::RemovePlayerState(APlayerState* PlayerState)
{
    Super::RemovePlayerState(PlayerState);
}

void AMvGameState::SeamlessTravelTransitionCheckpoint(bool bToTransitionMap)
{
    // Remove inactive and bots
    for (int32 i = PlayerArray.Num() - 1; i >= 0; i--)
    {
        APlayerState* PlayerState = PlayerArray[i];
        if (PlayerState && (PlayerState->IsABot() || PlayerState->IsInactive()))
        {
            RemovePlayerState(PlayerState);
        }
    }
}

UAbilitySystemComponent* AMvGameState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
