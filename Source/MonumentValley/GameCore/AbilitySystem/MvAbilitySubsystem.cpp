// Fill out your copyright notice in the Description page of Project Settings.


#include "MonumentValley/GameCore/AbilitySystem/MvAbilitySubsystem.h"

#include "MvAbilitySystemComponent.h"

//~Helper structs
void FGlobalAppliedAbilityList::AddToASC(TSubclassOf<UGameplayAbility> Ability, UMvAbilitySystemComponent* Comp)
{
    if (Comp2HandleMap.Find(Comp))
    {
        RemoveFromASC(Comp);
    }

    UGameplayAbility*          AbilityCDO  = Ability->GetDefaultObject<UGameplayAbility>();
    const FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityCDO);

    const FGameplayAbilitySpecHandle AbilitySpecHandle = Comp->GiveAbility(AbilitySpec);
    Comp2HandleMap.Add(Comp, AbilitySpecHandle);
}

void FGlobalAppliedAbilityList::RemoveFromASC(UMvAbilitySystemComponent* Comp)
{
    if (const FGameplayAbilitySpecHandle* SpecHandle = Comp2HandleMap.Find(Comp))
    {
        Comp->ClearAbility(*SpecHandle);
        Comp2HandleMap.Remove(Comp);
    }
}

void FGlobalAppliedAbilityList::RemoveFromAll()
{
    for (auto& Pair : Comp2HandleMap)
    {
        if (Pair.Key != nullptr)
        {
            Pair.Key->ClearAbility(Pair.Value);
        }
    }
    Comp2HandleMap.Empty();
}

void FGlobalAppliedEffectList::AddToASC(TSubclassOf<UGameplayEffect> Effect, UMvAbilitySystemComponent* Comp)
{
    if (Comp2HandleMap.Find(Comp))
    {
        RemoveFromASC(Comp);
    }

    const UGameplayEffect*            EffectCDO    = Effect->GetDefaultObject<UGameplayEffect>();
    const FActiveGameplayEffectHandle EffectHandle = Comp->ApplyGameplayEffectToSelf(EffectCDO, 1, Comp->MakeEffectContext());
    Comp2HandleMap.Add(Comp, EffectHandle);
}

void FGlobalAppliedEffectList::RemoveFromASC(UMvAbilitySystemComponent* Comp)
{
    if (const FActiveGameplayEffectHandle* EffectHandle = Comp2HandleMap.Find(Comp))
    {
        Comp->RemoveActiveGameplayEffect(*EffectHandle);
        Comp2HandleMap.Remove(Comp);
    }
}

void FGlobalAppliedEffectList::RemoveFromAll()
{
    for (auto& Pair : Comp2HandleMap)
    {
        if (Pair.Key != nullptr)
        {
            Pair.Key->RemoveActiveGameplayEffect(Pair.Value);
        }
    }
    Comp2HandleMap.Empty();
}
//~Helper structs

UMvAbilitySubsystem::UMvAbilitySubsystem()
{}

void UMvAbilitySubsystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)
{
    if ((Ability.Get() != nullptr) && (!AppliedAbilities.Contains(Ability)))
    {
        FGlobalAppliedAbilityList& Entry = AppliedAbilities.Add(Ability);
        for (UMvAbilitySystemComponent* ASC : RegisteredComps)
        {
            Entry.AddToASC(Ability, ASC);
        }
    }
}

void UMvAbilitySubsystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)
{
    if ((Effect.Get() != nullptr) && (!AppliedEffects.Contains(Effect)))
    {
        FGlobalAppliedEffectList& Entry = AppliedEffects.Add(Effect);
        for (UMvAbilitySystemComponent* ASC : RegisteredComps)
        {
            Entry.AddToASC(Effect, ASC);
        }
    }
}

void UMvAbilitySubsystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)
{
    if ((Ability.Get() != nullptr) && AppliedAbilities.Contains(Ability))
    {
        FGlobalAppliedAbilityList& Entry = AppliedAbilities[Ability];
        Entry.RemoveFromAll();
        AppliedAbilities.Remove(Ability);
    }
}

void UMvAbilitySubsystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)
{
    if ((Effect.Get() != nullptr) && AppliedEffects.Contains(Effect))
    {
        FGlobalAppliedEffectList& Entry = AppliedEffects[Effect];
        Entry.RemoveFromAll();
        AppliedEffects.Remove(Effect);
    }
}

void UMvAbilitySubsystem::RegisterASC(UMvAbilitySystemComponent* Comp)
{
    check(Comp);

    for (auto& Entry : AppliedAbilities)
    {
        Entry.Value.AddToASC(Entry.Key, Comp);
    }
    for (auto& Entry : AppliedEffects)
    {
        Entry.Value.AddToASC(Entry.Key, Comp);
    }

    RegisteredComps.AddUnique(Comp);
}

void UMvAbilitySubsystem::UnregisterASC(UMvAbilitySystemComponent* Comp)
{
    check(Comp);
    for (auto& Entry : AppliedAbilities)
    {
        Entry.Value.RemoveFromASC(Comp);
    }
    for (auto& Entry : AppliedEffects)
    {
        Entry.Value.RemoveFromASC(Comp);
    }

    RegisteredComps.Remove(Comp);
}
