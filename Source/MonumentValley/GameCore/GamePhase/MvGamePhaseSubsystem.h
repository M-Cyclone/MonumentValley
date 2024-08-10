// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"

#include "MvGamePhaseAbility.h"

#include "MvGamePhaseSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMvGamePhase, Log, All);

DECLARE_DYNAMIC_DELEGATE_OneParam(FMvGamePhaseDynamicDelegate, const UMvGamePhaseAbility*, Phase);
DECLARE_DELEGATE_OneParam(FMvGamePhaseDelegate, const UMvGamePhaseAbility* Phase);

DECLARE_DYNAMIC_DELEGATE_OneParam(FMvGamePhaseTagDynamicDelegate, const FGameplayTag&, PhaseTag);
DECLARE_DELEGATE_OneParam(FMvGamePhaseTagDelegate, const FGameplayTag& PhaseTag);

// Match rule for message receivers
UENUM(BlueprintType)
enum class EPhaseTagMatchType : uint8
{
    // An exact match will only receive messages with exactly the same channel
    // (e.g., registering for "A.B" will match a broadcast of A.B but not A.B.C)
    ExactMatch,

    // A partial match will receive any messages rooted in the same channel
    // (e.g., registering for "A.B" will match a broadcast of A.B as well as A.B.C)
    PartialMatch
};

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API UMvGamePhaseSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

    friend class UMvGamePhaseAbility;

public:
    UMvGamePhaseSubsystem();

    virtual void PostInitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

public:
    void StartPhase(TSubclassOf<UMvGamePhaseAbility> PhaseAbility, FMvGamePhaseDelegate PhaseEndedCallback = {});

    void WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FMvGamePhaseTagDelegate& WhenPhaseActive);
    void WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FMvGamePhaseTagDelegate& WhenPhaseEnd);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure = false, meta = (AutoCreateRefTerm = "PhaseTag"))
    bool IsPhaseActive(const FGameplayTag& PhaseTag) const;

    UFUNCTION(BlueprintCallable,
              BlueprintAuthorityOnly,
              Category = "Game Phase",
              meta     = (DisplayName = "Start Phase", AutoCreateRefTerm = "PhaseEnded"))
    void K2_StartPhase(TSubclassOf<UMvGamePhaseAbility> Phase, const FMvGamePhaseDynamicDelegate& PhaseEndedDelegate);

    UFUNCTION(BlueprintCallable,
              BlueprintAuthorityOnly,
              Category = "Game Phase",
              meta     = (DisplayName = "When Phase Starts or Is Active", AutoCreateRefTerm = "WhenPhaseActive"))
    void K2_WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FMvGamePhaseTagDynamicDelegate WhenPhaseActive);

    UFUNCTION(BlueprintCallable,
              BlueprintAuthorityOnly,
              Category = "Game Phase",
              meta     = (DisplayName = "When Phase Ends", AutoCreateRefTerm = "WhenPhaseEnd"))
    void K2_WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FMvGamePhaseTagDynamicDelegate WhenPhaseEnd);

protected:
    void OnBeginPhase(const UMvGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);
    void OnEndPhase(const UMvGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);

private:
    struct FMvGamePhaseEntry
    {
    public:
        FGameplayTag         PhaseTag;
        FMvGamePhaseDelegate PhaseEndedCallback;
    };

    /**
     * Can active multi phases at the same time.
     * i.e. Game.Battle + Game.Battle.Running
     */
    TMap<FGameplayAbilitySpecHandle, FMvGamePhaseEntry> ActivePhaseMap;

    struct FPhaseObserver
    {
    public:
        bool IsMatch(const FGameplayTag& ComparePhaseTag) const;

        FGameplayTag            PhaseTag;
        EPhaseTagMatchType      MatchType = EPhaseTagMatchType::ExactMatch;
        FMvGamePhaseTagDelegate PhaseCallback;
    };

    TArray<FPhaseObserver> PhaseStartObservers;
    TArray<FPhaseObserver> PhaseEndObservers;
};
