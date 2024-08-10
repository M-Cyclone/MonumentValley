// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"

#include "MvAbilityTagRelationshipMapping.h"
#include "MvGameplayAbility.h"

#include "MvAbilitySystemComponent.generated.h"

MONUMENTVALLEY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);

enum class EMvAbilityActivationGroup : uint8;

class UMvGameplayAbility;

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API UMvAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UMvAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
    virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

    //~UActorComponent interface
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of UActorComponent interface

    using TShouldCancelAbilityFunc = TFunctionRef<bool(const UMvGameplayAbility* LyraAbility, FGameplayAbilitySpecHandle Handle)>;
    void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);

    void CancelInputActivatedAbilities(bool bReplicateCancelAbility);

    //~Input process
public:
    void AbilityInputTagPressed(const FGameplayTag& InputTag);
    void AbilityInputTagReleased(const FGameplayTag& InputTag);

    void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
    void ClearAbilityInput();
    //~Input process

public:
    bool IsActivationGroupBlocked(EMvAbilityActivationGroup Group) const;
    void AddAbilityToActivationGroup(EMvAbilityActivationGroup Group, UMvGameplayAbility* LyraAbility);
    void RemoveAbilityFromActivationGroup(EMvAbilityActivationGroup Group, UMvGameplayAbility* LyraAbility);
    void CancelActivationGroupAbilities(EMvAbilityActivationGroup Group,
                                        UMvGameplayAbility*       IgnoreLyraAbility,
                                        bool                      bReplicateCancelAbility);

protected:
    void TryActivateAbilitiesOnSpawn();

    //~UAbilitySystemComponent
    virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
    virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

    virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
    virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle,
                                     UGameplayAbility*                Ability,
                                     const FGameplayTagContainer&     FailureReason) override;
    virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
    virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
                                                UGameplayAbility*            RequestingAbility,
                                                bool                         bEnableBlockTags,
                                                const FGameplayTagContainer& BlockTags,
                                                bool                         bExecuteCancelTags,
                                                const FGameplayTagContainer& CancelTags) override;
    virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags,
                                                  UGameplayAbility*            RequestingAbility,
                                                  bool                         bCanBeCanceled) override;
    //~UAbilitySystemComponent

    /** Notify client that an ability failed to activate */
    UFUNCTION(Client, Unreliable)
    void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

    void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

protected:
    // If set, this table is used to look up tag relationships for activate and cancel
    UPROPERTY()
    TObjectPtr<UMvAbilityTagRelationshipMapping> TagRelationshipMapping;

    // Handles to abilities that had their input pressed this frame.
    TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

    // Handles to abilities that had their input released this frame.
    TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

    // Handles to abilities that have their input held.
    TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

    // Number of abilities running in each activation group.
    int32 ActivationGroupCounts[(uint8)EMvAbilityActivationGroup::Max];
};
