// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"
#include "GameplayAbilitySpecHandle.h"

#include "Subsystems/WorldSubsystem.h"

#include "MvAbilitySubsystem.generated.h"
class UMvAbilitySystemComponent;

/**
 * Map: ASC -> Ability Handle
 */
USTRUCT()
struct FGlobalAppliedAbilityList
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<TObjectPtr<UMvAbilitySystemComponent>, FGameplayAbilitySpecHandle> Comp2HandleMap;

    void AddToASC(TSubclassOf<UGameplayAbility> Ability, UMvAbilitySystemComponent* Comp);
    void RemoveFromASC(UMvAbilitySystemComponent* Comp);
    void RemoveFromAll();
};

USTRUCT()
struct FGlobalAppliedEffectList
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<TObjectPtr<UMvAbilitySystemComponent>, FActiveGameplayEffectHandle> Comp2HandleMap;

    void AddToASC(TSubclassOf<UGameplayEffect> Effect, UMvAbilitySystemComponent* Comp);
    void RemoveFromASC(UMvAbilitySystemComponent* Comp);
    void RemoveFromAll();
};

/**
 * Monument Valley game's ability manager subsystem.
 */
UCLASS()
class MONUMENTVALLEY_API UMvAbilitySubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UMvAbilitySubsystem();
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Monument Valley")
    void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Monument Valley")
    void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Monument Valley")
    void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Monument Valley")
    void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

    /** Register an ASC with global system and apply any active global effects/abilities. */
    void RegisterASC(UMvAbilitySystemComponent* Comp);

    /** Removes an ASC from the global system, along with any active global effects/abilities. */
    void UnregisterASC(UMvAbilitySystemComponent* Comp);

private:
    /**
     * Map: AbilityClass -> Map<ASC, AbilityHandle>
     * 
     * Usage: auto AbilityHandle = AppliedAbilities[Ability::GetClass()][TargetASC];
     */
    UPROPERTY()
    TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

    /**
     * Map: EffectClass -> Map<ASC, EffectHandle>
     * 
     * Usage: auto EffectHandle = AppliedEffects[Effect::GetClass()][TargetASC];
     */
    UPROPERTY()
    TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

    // Array of all ASC.
    UPROPERTY()
    TArray<TObjectPtr<UMvAbilitySystemComponent>> RegisteredComps;
};
