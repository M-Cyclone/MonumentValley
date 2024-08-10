// Fill out your copyright notice in the Description page of Project Settings.


#include "MonumentValley/GameCore/GamePhase/MvGamePhaseSubsystem.h"

#include "GameplayTagContainer.h"

#include "GameFramework/GameStateBase.h"

#include "MonumentValley/GameCore/AbilitySystem/MvAbilitySystemComponent.h"

#include "MvGamePhaseAbility.h"

DEFINE_LOG_CATEGORY(LogMvGamePhase);

UMvGamePhaseSubsystem::UMvGamePhaseSubsystem()
{}

void UMvGamePhaseSubsystem::PostInitialize()
{
    Super::PostInitialize();
}

bool UMvGamePhaseSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return Super::ShouldCreateSubsystem(Outer);
}

bool UMvGamePhaseSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UMvGamePhaseSubsystem::StartPhase(TSubclassOf<UMvGamePhaseAbility> PhaseAbility, FMvGamePhaseDelegate PhaseEndedCallback)
{
    UWorld*                    World = GetWorld();
    UMvAbilitySystemComponent* Comp  = World->GetGameState()->FindComponentByClass<UMvAbilitySystemComponent>();
    if (ensure(Comp))
    {
        FGameplayAbilitySpec       PhaseSpec(PhaseAbility, 1, 0, this);
        FGameplayAbilitySpecHandle SpecHandle = Comp->GiveAbilityAndActivateOnce(PhaseSpec);
        FGameplayAbilitySpec*      FoundSpec  = Comp->FindAbilitySpecFromHandle(SpecHandle);

        if (FoundSpec && FoundSpec->IsActive())
        {
            FMvGamePhaseEntry& Entry = ActivePhaseMap.FindOrAdd(SpecHandle);
            Entry.PhaseEndedCallback = PhaseEndedCallback;
        }
        else
        {
            PhaseEndedCallback.ExecuteIfBound(nullptr);
        }
    }
}

void UMvGamePhaseSubsystem::WhenPhaseStartsOrIsActive(FGameplayTag                   PhaseTag,
                                                      EPhaseTagMatchType             MatchType,
                                                      const FMvGamePhaseTagDelegate& WhenPhaseActive)
{
    FPhaseObserver Observer;
    Observer.PhaseTag      = PhaseTag;
    Observer.MatchType     = MatchType;
    Observer.PhaseCallback = WhenPhaseActive;
    PhaseStartObservers.Add(Observer);

    if (IsPhaseActive(PhaseTag))
    {
        WhenPhaseActive.ExecuteIfBound(PhaseTag);
    }
}

void UMvGamePhaseSubsystem::WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FMvGamePhaseTagDelegate& WhenPhaseEnd)
{
    FPhaseObserver Observer;
    Observer.PhaseTag      = PhaseTag;
    Observer.MatchType     = MatchType;
    Observer.PhaseCallback = WhenPhaseEnd;
    PhaseEndObservers.Add(Observer);
}

bool UMvGamePhaseSubsystem::IsPhaseActive(const FGameplayTag& PhaseTag) const
{
    for (const auto& Pair : ActivePhaseMap)
    {
        const FMvGamePhaseEntry& PhaseEntry = Pair.Value;
        if (PhaseEntry.PhaseTag.MatchesTag(PhaseTag))
        {
            return true;
        }
    }

    return false;
}

void UMvGamePhaseSubsystem::K2_StartPhase(TSubclassOf<UMvGamePhaseAbility> Phase, const FMvGamePhaseDynamicDelegate& PhaseEndedDelegate)
{
    const FMvGamePhaseDelegate EndedDelegate = FMvGamePhaseDelegate::CreateWeakLambda(
        const_cast<UObject*>(PhaseEndedDelegate.GetUObject()),
        [PhaseEndedDelegate](const UMvGamePhaseAbility* PhaseAbility) { PhaseEndedDelegate.ExecuteIfBound(PhaseAbility); });

    StartPhase(Phase, EndedDelegate);
}

void UMvGamePhaseSubsystem::K2_WhenPhaseStartsOrIsActive(FGameplayTag                   PhaseTag,
                                                         EPhaseTagMatchType             MatchType,
                                                         FMvGamePhaseTagDynamicDelegate WhenPhaseActive)
{
    const FMvGamePhaseTagDelegate ActiveDelegate = FMvGamePhaseTagDelegate::CreateWeakLambda(
        WhenPhaseActive.GetUObject(), [WhenPhaseActive](const FGameplayTag& PhaseTag) { WhenPhaseActive.ExecuteIfBound(PhaseTag); });

    WhenPhaseStartsOrIsActive(PhaseTag, MatchType, ActiveDelegate);
}

void UMvGamePhaseSubsystem::K2_WhenPhaseEnds(FGameplayTag                   PhaseTag,
                                             EPhaseTagMatchType             MatchType,
                                             FMvGamePhaseTagDynamicDelegate WhenPhaseEnd)
{
    const FMvGamePhaseTagDelegate EndedDelegate = FMvGamePhaseTagDelegate::CreateWeakLambda(
        WhenPhaseEnd.GetUObject(), [WhenPhaseEnd](const FGameplayTag& PhaseTag) { WhenPhaseEnd.ExecuteIfBound(PhaseTag); });

    WhenPhaseEnds(PhaseTag, MatchType, EndedDelegate);
}

void UMvGamePhaseSubsystem::OnBeginPhase(const UMvGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle)
{
    const FGameplayTag TargetPhaseTag = PhaseAbility->GetGamePhaseTag();
    UE_LOG(LogMvGamePhase, Log, TEXT("Beginning Phase '%s' (%s)"), *TargetPhaseTag.ToString(), *GetNameSafe(PhaseAbility));

    const UWorld* World = GetWorld();
    if (UMvAbilitySystemComponent* Comp = World->GetGameState()->FindComponentByClass<UMvAbilitySystemComponent>(); ensure(Comp))
    {
        TArray<FGameplayAbilitySpec*> ActivePhases;
        for (const auto& Pair : ActivePhaseMap)
        {
            const FGameplayAbilitySpecHandle ActiveAbilityHandle = Pair.Key;
            if (FGameplayAbilitySpec* Spec = Comp->FindAbilitySpecFromHandle(ActiveAbilityHandle))
            {
                ActivePhases.Add(Spec);
            }
        }

        for (const FGameplayAbilitySpec* ActivePhase : ActivePhases)
        {
            const UMvGamePhaseAbility* ActivePhaseAbility = CastChecked<UMvGamePhaseAbility>(ActivePhase->Ability);
            const FGameplayTag         ActivePhaseTag     = ActivePhaseAbility->GetGamePhaseTag();

            // So if the active phase currently matches the incoming phase tag, we allow it.
            // i.e. multiple gameplay abilities can all be associated with the same phase tag.
            // For example,
            // You can be in the, Game.Playing, phase, and then start a sub-phase, like Game.Playing.SuddenDeath
            // Game.Playing phase will still be active, and if someone were to push another one, like,
            // Game.Playing.ActualSuddenDeath, it would end Game.Playing.SuddenDeath phase, but Game.Playing would
            // continue.  Similarly if we activated Game.GameOver, all the Game.Playing* phases would end.
            if (!TargetPhaseTag.MatchesTag(ActivePhaseTag))
            {
                UE_LOG(LogMvGamePhase, Log, TEXT("\tEnding Phase '%s' (%s)"), *ActivePhaseTag.ToString(), *GetNameSafe(ActivePhaseAbility));

                FGameplayAbilitySpecHandle HandleToEnd = ActivePhase->Handle;
                Comp->CancelAbilitiesByFunc(
                    [HandleToEnd](const UMvGameplayAbility*, FGameplayAbilitySpecHandle Handle) { return Handle == HandleToEnd; }, true);
            }
        }

        FMvGamePhaseEntry& Entry = ActivePhaseMap.FindOrAdd(PhaseAbilityHandle);
        Entry.PhaseTag           = TargetPhaseTag;

        // Notify all observers of this phase that it has started.
        for (const FPhaseObserver& Observer : PhaseStartObservers)
        {
            if (Observer.IsMatch(TargetPhaseTag))
            {
                Observer.PhaseCallback.ExecuteIfBound(TargetPhaseTag);
            }
        }
    }
}

void UMvGamePhaseSubsystem::OnEndPhase(const UMvGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle)
{
    const FGameplayTag EndedPhaseTag = PhaseAbility->GetGamePhaseTag();
    UE_LOG(LogMvGamePhase, Log, TEXT("Ended Phase '%s' (%s)"), *EndedPhaseTag.ToString(), *GetNameSafe(PhaseAbility));

    const FMvGamePhaseEntry& Entry = ActivePhaseMap.FindChecked(PhaseAbilityHandle);
    Entry.PhaseEndedCallback.ExecuteIfBound(PhaseAbility);

    ActivePhaseMap.Remove(PhaseAbilityHandle);

    // Notify all observers of this phase that it has ended.
    for (const FPhaseObserver& Observer : PhaseEndObservers)
    {
        if (Observer.IsMatch(EndedPhaseTag))
        {
            Observer.PhaseCallback.ExecuteIfBound(EndedPhaseTag);
        }
    }
}

bool UMvGamePhaseSubsystem::FPhaseObserver::IsMatch(const FGameplayTag& ComparePhaseTag) const
{
    switch (MatchType)
    {
    case EPhaseTagMatchType::ExactMatch: return ComparePhaseTag == PhaseTag;
    case EPhaseTagMatchType::PartialMatch: return ComparePhaseTag.MatchesTag(PhaseTag);
    }

    return false;
}
