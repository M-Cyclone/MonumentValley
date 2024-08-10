// Fill out your copyright notice in the Description page of Project Settings.


#include "MonumentValley/GameCore/AbilitySystem/MvGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "MonumentValley/MvLogChannels.h"

UMvGameplayAbility::UMvGameplayAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

bool UMvGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayTagContainer*     SourceTags,
                                            const FGameplayTagContainer*     TargetTags,
                                            FGameplayTagContainer*           OptionalRelevantTags) const
{
    if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
    {
        return false;
    }

    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    return true;
}

void UMvGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
    // The ability can not block canceling if it's replaceable.
    if (!bCanBeCanceled && (ActivationGroup == EMvAbilityActivationGroup::Exclusive_Replaceable))
    {
        UE_LOG(LogMvAbilitySystem,
               Error,
               TEXT("SetCanBeCanceled: Ability [%s] can not block canceling because its activation group is replaceable."),
               *GetName());
        return;
    }

    Super::SetCanBeCanceled(bCanBeCanceled);
}

void UMvGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    K2_OnAbilityAdded();

    TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UMvGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    K2_OnAbilityRemoved();

    Super::OnRemoveAbility(ActorInfo, Spec);
}

void UMvGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle     Handle,
                                         const FGameplayAbilityActorInfo*     ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData*            TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMvGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle     Handle,
                                    const FGameplayAbilityActorInfo*     ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                    bool                                 bReplicateEndAbility,
                                    bool                                 bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UMvGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
                                   const FGameplayAbilityActorInfo* ActorInfo,
                                   FGameplayTagContainer*           OptionalRelevantTags) const
{
    // Base ability class needs not cost.
    return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

void UMvGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle     Handle,
                                   const FGameplayAbilityActorInfo*     ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo) const
{
    // Base ability class needs not cost.
    Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

FGameplayEffectContextHandle UMvGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
                                                                   const FGameplayAbilityActorInfo* ActorInfo) const
{
    FGameplayEffectContextHandle ContextHandle = Super::MakeEffectContext(Handle, ActorInfo);

    // FLyraGameplayEffectContext* EffectContext = FLyraGameplayEffectContext::ExtractEffectContext(ContextHandle);
    // check(EffectContext);
    //
    // check(ActorInfo);
    //
    // AActor*                            EffectCauser  = nullptr;
    // const ILyraAbilitySourceInterface* AbilitySource = nullptr;
    // float                              SourceLevel   = 0.0f;
    // GetAbilitySource(Handle, ActorInfo, /*out*/ SourceLevel, /*out*/ AbilitySource, /*out*/ EffectCauser);
    //
    // UObject* SourceObject = GetSourceObject(Handle, ActorInfo);
    //
    // AActor* Instigator = ActorInfo ? ActorInfo->OwnerActor.Get() : nullptr;
    //
    // EffectContext->SetAbilitySource(AbilitySource, SourceLevel);
    // EffectContext->AddInstigator(Instigator, EffectCauser);
    // EffectContext->AddSourceObject(SourceObject);

    return ContextHandle;
}

void UMvGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const
{
    Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);
}

bool UMvGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
                                                           const FGameplayTagContainer*   SourceTags,
                                                           const FGameplayTagContainer*   TargetTags,
                                                           FGameplayTagContainer*         OptionalRelevantTags) const
{
    return Super::DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, SourceTags, TargetTags, OptionalRelevantTags);
}

void UMvGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
    const bool bIsPredicting      = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);
    const bool bIsSpecActive      = Spec.IsActive();
    const bool bIsOnSpawnActivate = ActivationPolicy == EMvAbilityActivationPolicy::OnSpawn;

    // Try to activate if activation policy is on spawn.
    if (ActorInfo && !bIsSpecActive && !bIsPredicting && bIsOnSpawnActivate)
    {
        UAbilitySystemComponent* Asc         = ActorInfo->AbilitySystemComponent.Get();
        const AActor*            AvatarActor = ActorInfo->AvatarActor.Get();

        // If avatar actor is torn off or about to die, don't try to activate until we get the new one.
        if (Asc && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
        {
            const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) ||
                                           (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
            const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) ||
                                            (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

            const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
            const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

            if (bClientShouldActivate || bServerShouldActivate)
            {
                Asc->TryActivateAbility(Spec.Handle);
            }
        }
    }
}

void UMvGameplayAbility::OnPawnAvatarSet()
{
    K2_OnPawnAvatarSet();
}
