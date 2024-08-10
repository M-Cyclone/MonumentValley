// Fill out your copyright notice in the Description page of Project Settings.


#include "MOnumentValley/GameCore/AbilitySystem/MvAbilitySystemComponent.h"

#include "MonumentValley/MvLogChannels.h"
#include "MvAbilitySubsystem.h"
#include "MvGameplayAbility.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked");

UMvAbilitySystemComponent::UMvAbilitySystemComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

void UMvAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
    Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

    FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
    check(ActorInfo);
    check(InOwnerActor);

    const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);
    if (!bHasNewPawnAvatar)
    {
        return;
    }

    // Notify all abilities that a new pawn avatar has been set
    for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
    {
        UMvGameplayAbility* MvAbilityCDO = Cast<UMvGameplayAbility>(AbilitySpec.Ability);
        if (!MvAbilityCDO)
        {
            continue;
        }

        if (MvAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
        {
            TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
            for (UGameplayAbility* AbilityInstance : Instances)
            {
                if (UMvGameplayAbility* GameplayAbility = Cast<UMvGameplayAbility>(AbilityInstance))
                {
                    // Ability instances may be missing for replays
                    GameplayAbility->OnPawnAvatarSet();
                }
            }
        }
        else
        {
            MvAbilityCDO->OnPawnAvatarSet();
        }
    }
}

void UMvAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UMvAbilitySubsystem* GlobalAbilitySystem = UWorld::GetSubsystem<UMvAbilitySubsystem>(GetWorld()))
    {
        GlobalAbilitySystem->UnregisterASC(this);
    }

    Super::EndPlay(EndPlayReason);
}

void UMvAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
    ABILITYLIST_SCOPE_LOCK();
    for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
    {
        if (!AbilitySpec.IsActive())
        {
            continue;
        }

        UMvGameplayAbility* LyraAbilityCDO = Cast<UMvGameplayAbility>(AbilitySpec.Ability);
        if (!LyraAbilityCDO)
        {
            UE_LOG(LogMvAbilitySystem,
                   Error,
                   TEXT("CancelAbilitiesByFunc: Non-LyraGameplayAbility %s was Granted to ASC. Skipping."),
                   *AbilitySpec.Ability.GetName());
            continue;
        }

        if (LyraAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
        {
            // Cancel all the spawned instances, not the CDO.
            TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
            for (UGameplayAbility* AbilityInstance : Instances)
            {
                UMvGameplayAbility* LyraAbilityInstance = CastChecked<UMvGameplayAbility>(AbilityInstance);

                if (ShouldCancelFunc(LyraAbilityInstance, AbilitySpec.Handle))
                {
                    if (LyraAbilityInstance->CanBeCanceled())
                    {
                        LyraAbilityInstance->CancelAbility(AbilitySpec.Handle,
                                                           AbilityActorInfo.Get(),
                                                           LyraAbilityInstance->GetCurrentActivationInfo(),
                                                           bReplicateCancelAbility);
                    }
                    else
                    {
                        UE_LOG(LogMvAbilitySystem,
                               Error,
                               TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."),
                               *LyraAbilityInstance->GetName());
                    }
                }
            }
        }
        else
        {
            // Cancel the non-instanced ability CDO.
            if (ShouldCancelFunc(LyraAbilityCDO, AbilitySpec.Handle))
            {
                // Non-instanced abilities can always be canceled.
                check(LyraAbilityCDO->CanBeCanceled());
                LyraAbilityCDO->CancelAbility(
                    AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
            }
        }
    }
}

void UMvAbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)
{
    auto ShouldCancelFunc = [this](const UMvGameplayAbility* LyraAbility, FGameplayAbilitySpecHandle Handle)
    {
        const EMvAbilityActivationPolicy ActivationPolicy = LyraAbility->GetActivationPolicy();
        return ((ActivationPolicy == EMvAbilityActivationPolicy::OnInputTriggered) ||
                (ActivationPolicy == EMvAbilityActivationPolicy::WhileInputActive));
    };

    CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

void UMvAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
    if (InputTag.IsValid())
    {
        for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
        {
            if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
            {
                InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
                InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
            }
        }
    }
}

void UMvAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
    if (InputTag.IsValid())
    {
        for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
        {
            if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
            {
                InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
                InputHeldSpecHandles.Remove(AbilitySpec.Handle);
            }
        }
    }
}

void UMvAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
    if (HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
    {
        ClearAbilityInput();
        return;
    }

    TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

    // Process all abilities that activate when the input is held.
    for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
    {
        if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
        {
            if (AbilitySpec->Ability && !AbilitySpec->IsActive())
            {
                const UMvGameplayAbility* AbilityCDO = Cast<UMvGameplayAbility>(AbilitySpec->Ability);
                if (AbilityCDO && AbilityCDO->GetActivationPolicy() == EMvAbilityActivationPolicy::WhileInputActive)
                {
                    AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
                }
            }
        }
    }

    // Process all abilities that had their input pressed this frame.
    for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
    {
        if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
        {
            if (AbilitySpec->Ability)
            {
                AbilitySpec->InputPressed = true;

                if (AbilitySpec->IsActive())
                {
                    // Ability is active so pass along the input event.
                    AbilitySpecInputPressed(*AbilitySpec);
                }
                else
                {
                    const UMvGameplayAbility* AbilityCDO = Cast<UMvGameplayAbility>(AbilitySpec->Ability);

                    if (AbilityCDO && AbilityCDO->GetActivationPolicy() == EMvAbilityActivationPolicy::OnInputTriggered)
                    {
                        AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
                    }
                }
            }
        }
    }

    // Try to activate all the abilities that are from presses and holds.
    // We do it all at once so that held inputs don't activate the ability
    // and then also send a input event to the ability because of the press.
    for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
    {
        TryActivateAbility(AbilitySpecHandle);
    }

    // Process all abilities that had their input released this frame.
    for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
    {
        if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
        {
            if (AbilitySpec->Ability)
            {
                AbilitySpec->InputPressed = false;

                if (AbilitySpec->IsActive())
                {
                    // Ability is active so pass along the input event.
                    AbilitySpecInputReleased(*AbilitySpec);
                }
            }
        }
    }

    // Clear the cached ability handles.
    InputPressedSpecHandles.Reset();
    InputReleasedSpecHandles.Reset();
}

void UMvAbilitySystemComponent::ClearAbilityInput()
{
    InputPressedSpecHandles.Reset();
    InputReleasedSpecHandles.Reset();
    InputHeldSpecHandles.Reset();
}

bool UMvAbilitySystemComponent::IsActivationGroupBlocked(EMvAbilityActivationGroup Group) const
{
    bool bBlocked = false;

    switch (Group)
    {
    case EMvAbilityActivationGroup::Independent:
        // Independent abilities are never blocked.
        bBlocked = false;
        break;

    case EMvAbilityActivationGroup::Exclusive_Replaceable:
    case EMvAbilityActivationGroup::Exclusive_Blocking:
        // Exclusive abilities can activate if nothing is blocking.
        bBlocked = (ActivationGroupCounts[(uint8)EMvAbilityActivationGroup::Exclusive_Blocking] > 0);
        break;

    default: checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), (uint8)Group); break;
    }

    return bBlocked;
}

void UMvAbilitySystemComponent::AddAbilityToActivationGroup(EMvAbilityActivationGroup Group, UMvGameplayAbility* LyraAbility)
{
    check(LyraAbility);
    check(ActivationGroupCounts[(uint8)Group] < INT32_MAX);

    ActivationGroupCounts[(uint8)Group]++;

    const bool bReplicateCancelAbility = false;

    switch (Group)
    {
    case EMvAbilityActivationGroup::Independent:
        // Independent abilities do not cancel any other abilities.
        break;

    case EMvAbilityActivationGroup::Exclusive_Replaceable:
    case EMvAbilityActivationGroup::Exclusive_Blocking:
        CancelActivationGroupAbilities(EMvAbilityActivationGroup::Exclusive_Replaceable, LyraAbility, bReplicateCancelAbility);
        break;

    default: checkf(false, TEXT("AddAbilityToActivationGroup: Invalid ActivationGroup [%d]\n"), (uint8)Group); break;
    }

    const int32 ExclusiveCount = ActivationGroupCounts[(uint8)EMvAbilityActivationGroup::Exclusive_Replaceable] +
                                 ActivationGroupCounts[(uint8)EMvAbilityActivationGroup::Exclusive_Blocking];
    if (!ensure(ExclusiveCount <= 1))
    {
        UE_LOG(LogMvAbilitySystem, Error, TEXT("AddAbilityToActivationGroup: Multiple exclusive abilities are running."));
    }
}

void UMvAbilitySystemComponent::RemoveAbilityFromActivationGroup(EMvAbilityActivationGroup Group, UMvGameplayAbility* LyraAbility)
{
    check(LyraAbility);
    check(ActivationGroupCounts[(uint8)Group] > 0);

    ActivationGroupCounts[(uint8)Group]--;
}

void UMvAbilitySystemComponent::CancelActivationGroupAbilities(EMvAbilityActivationGroup Group,
                                                               UMvGameplayAbility*       IgnoreLyraAbility,
                                                               bool                      bReplicateCancelAbility)
{
    auto ShouldCancelFunc = [this, Group, IgnoreLyraAbility](const UMvGameplayAbility* LyraAbility, FGameplayAbilitySpecHandle Handle)
    {
        return ((LyraAbility->GetActivationGroup() == Group) && (LyraAbility != IgnoreLyraAbility));
    };

    CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

void UMvAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
    ABILITYLIST_SCOPE_LOCK();
    for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
    {
        if (const UMvGameplayAbility* AbilityCDO = Cast<UMvGameplayAbility>(AbilitySpec.Ability))
        {
            AbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
        }
    }
}

void UMvAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
    Super::AbilitySpecInputPressed(Spec);

    // We don't support UGameplayAbility::bReplicateInputDirectly.
    // Use replicated events instead so that the WaitInputPress ability task works.
    if (Spec.IsActive())
    {
        // Invoke the InputPressed event. This is not replicated here.
        // If someone is listening, they may replicate the InputPressed event to the server.
        InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
    }
}

void UMvAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
    Super::AbilitySpecInputReleased(Spec);

    // We don't support UGameplayAbility::bReplicateInputDirectly.
    // Use replicated events instead so that the WaitInputRelease ability task works.
    if (Spec.IsActive())
    {
        // Invoke the InputReleased event. This is not replicated here.
        // If someone is listening, they may replicate the InputReleased event to the server.
        InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
    }
}

void UMvAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
    Super::NotifyAbilityActivated(Handle, Ability);

    if (UMvGameplayAbility* MvAbility = Cast<UMvGameplayAbility>(Ability))
    {
        AddAbilityToActivationGroup(MvAbility->GetActivationGroup(), MvAbility);
    }
}

void UMvAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle,
                                                    UGameplayAbility*                Ability,
                                                    const FGameplayTagContainer&     FailureReason)
{
    Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

    if (const APawn* Avatar = Cast<APawn>(GetAvatarActor()))
    {
        if (!Avatar->IsLocallyControlled() && Ability->IsSupportedForNetworking())
        {
            ClientNotifyAbilityFailed(Ability, FailureReason);
            return;
        }
    }

    HandleAbilityFailed(Ability, FailureReason);
}

void UMvAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
    Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

    if (UMvGameplayAbility* MvAbility = Cast<UMvGameplayAbility>(Ability))
    {
        RemoveAbilityFromActivationGroup(MvAbility->GetActivationGroup(), MvAbility);
    }
}

void UMvAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
                                                               UGameplayAbility*            RequestingAbility,
                                                               bool                         bEnableBlockTags,
                                                               const FGameplayTagContainer& BlockTags,
                                                               bool                         bExecuteCancelTags,
                                                               const FGameplayTagContainer& CancelTags)
{
    FGameplayTagContainer ModifiedBlockTags  = BlockTags;
    FGameplayTagContainer ModifiedCancelTags = CancelTags;

    Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, BlockTags, bExecuteCancelTags, CancelTags);
}

void UMvAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags,
                                                                 UGameplayAbility*            RequestingAbility,
                                                                 bool                         bCanBeCanceled)
{
    Super::HandleChangeAbilityCanBeCanceled(AbilityTags, RequestingAbility, bCanBeCanceled);
}

void UMvAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
    if (const UMvGameplayAbility* MvAbility = Cast<const UMvGameplayAbility>(Ability))
    {
        MvAbility->OnAbilityFailedToActivate(FailureReason);
    }
}

void UMvAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility*      Ability,
                                                                         const FGameplayTagContainer& FailureReason)
{
    HandleAbilityFailed(Ability, FailureReason);
}
