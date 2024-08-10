// Fill out your copyright notice in the Description page of Project Settings.


#include "MonumentValley/GameCore/Player/MvPlayerState.h"

#include "MvPlayerController.h"

AMvPlayerState::AMvPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UMvAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

AMvPlayerController* AMvPlayerState::GetLyraPlayerController() const
{
    return Cast<AMvPlayerController>(GetOwner());
}

UAbilitySystemComponent* AMvPlayerState::GetAbilitySystemComponent() const
{
    return GetMvAbilitySystemComponent();
}

void AMvPlayerState::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AMvPlayerState::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void AMvPlayerState::Reset()
{
    Super::Reset();
}

void AMvPlayerState::ClientInitialize(AController* C)
{
    Super::ClientInitialize(C);
}

void AMvPlayerState::CopyProperties(APlayerState* PlayerState)
{
    Super::CopyProperties(PlayerState);
}

void AMvPlayerState::OnDeactivated()
{
    Super::OnDeactivated();
}

void AMvPlayerState::OnReactivated()
{
    Super::OnReactivated();
}
