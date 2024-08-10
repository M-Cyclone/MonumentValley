// Fill out your copyright notice in the Description page of Project Settings.

#include "MonumentValley/GameCore/GamePhase/MvGamePhaseAbility.h"

#include "AbilitySystemComponent.h"
#include "MvGamePhaseSubsystem.h"

UMvGamePhaseAbility::UMvGamePhaseAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

#if WITH_EDITOR
EDataValidationResult UMvGamePhaseAbility::IsDataValid(FDataValidationContext& Context) const
{
    return Super::IsDataValid(Context);
}
#endif

void UMvGamePhaseAbility::ActivateAbility(const FGameplayAbilitySpecHandle     Handle,
                                          const FGameplayAbilityActorInfo*     ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData*            TriggerEventData)
{
    const UWorld*          World              = ActorInfo->AbilitySystemComponent->GetWorld();
    UMvGamePhaseSubsystem* GamePhaseSubsystem = UWorld::GetSubsystem<UMvGamePhaseSubsystem>(World);
    GamePhaseSubsystem->OnBeginPhase(this, Handle);

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMvGamePhaseAbility::EndAbility(const FGameplayAbilitySpecHandle     Handle,
                                     const FGameplayAbilityActorInfo*     ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     bool                                 bReplicateEndAbility,
                                     bool                                 bWasCancelled)
{
    const UWorld*          World              = ActorInfo->AbilitySystemComponent->GetWorld();
    UMvGamePhaseSubsystem* GamePhaseSubsystem = UWorld::GetSubsystem<UMvGamePhaseSubsystem>(World);
    GamePhaseSubsystem->OnEndPhase(this, Handle);

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
