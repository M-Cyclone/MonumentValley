// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"

#include "MvGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EMvAbilityActivationPolicy : uint8
{
    // Try to activate the ability when the input is triggered.
    OnInputTriggered,

    // Continually try to activate the ability while the input is active.
    WhileInputActive,

    // Try to activate the ability when an avatar is assigned.
    OnSpawn
};

UENUM(BlueprintType)
enum class EMvAbilityActivationGroup : uint8
{
    // Ability runs independently of all other abilities.
    Independent,

    // Ability is canceled and replaced by other exclusive abilities.
    Exclusive_Replaceable,

    // Ability blocks all other exclusive abilities from activating.
    Exclusive_Blocking,

    Max UMETA(Hidden)
};

/**
 * Monument Valley game's basic GA class.
 */
UCLASS()
class MONUMENTVALLEY_API UMvGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()
    friend class UMvAbilitySystemComponent;

public:
    UMvGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    //~UGameplayAbility interface
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayTagContainer*     SourceTags,
                                    const FGameplayTagContainer*     TargetTags,
                                    FGameplayTagContainer*           OptionalRelevantTags) const override;
    virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle     Handle,
                                 const FGameplayAbilityActorInfo*     ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData*            TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle     Handle,
                            const FGameplayAbilityActorInfo*     ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool                                 bReplicateEndAbility,
                            bool                                 bWasCancelled) override;
    virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle,
                           const FGameplayAbilityActorInfo* ActorInfo,
                           FGameplayTagContainer*           OptionalRelevantTags = nullptr) const override;
    virtual void ApplyCost(const FGameplayAbilitySpecHandle     Handle,
                           const FGameplayAbilityActorInfo*     ActorInfo,
                           const FGameplayAbilityActivationInfo ActivationInfo) const override;
    virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
                                                           const FGameplayAbilityActorInfo* ActorInfo) const override;
    virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
    virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
                                                   const FGameplayTagContainer*   SourceTags           = nullptr,
                                                   const FGameplayTagContainer*   TargetTags           = nullptr,
                                                   FGameplayTagContainer*         OptionalRelevantTags = nullptr) const override;
    //~End of UGameplayAbility interface

public:
    void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
    {
        // NativeOnAbilityFailedToActivate(FailedReason);
        ScriptOnAbilityFailedToActivate(FailedReason);
    }

protected:
    // // Called when the ability fails to activate
    // virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

    // Called when the ability fails to activate
    UFUNCTION(BlueprintImplementableEvent)
    void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

protected:
    void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

protected:
    EMvAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
    EMvAbilityActivationGroup  GetActivationGroup() const { return ActivationGroup; }

protected:
    // Defines how this ability is meant to activate.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MonumentValley|Ability Activation")
    EMvAbilityActivationPolicy ActivationPolicy;

    // Defines the relationship between this ability activating and other abilities activating.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MonumentValley|Ability Activation")
    EMvAbilityActivationGroup ActivationGroup;

protected:
    virtual void OnPawnAvatarSet();

    //~Blueprint callback functions.
protected:
    /** Called when this ability is granted to the ability system component. */
    UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
    void K2_OnAbilityAdded();

    /** Called when this ability is removed from the ability system component. */
    UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
    void K2_OnAbilityRemoved();

    /** Called when the ability system is initialized with a pawn avatar. */
    UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
    void K2_OnPawnAvatarSet();
    //~Blueprint callback functions.
};
