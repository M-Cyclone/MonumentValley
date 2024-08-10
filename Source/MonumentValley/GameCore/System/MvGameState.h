// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"
#include "GameFramework/GameStateBase.h"

#include "MonumentValley/GameCore/AbilitySystem/MvAbilitySystemComponent.h"

#include "MvGameState.generated.h"

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API AMvGameState
    : public AGameStateBase
    , public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AMvGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    //~AActor interface
    virtual void PreInitializeComponents() override;
    virtual void PostInitializeComponents() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaSeconds) override;
    //~End of AActor interface

    //~AGameStateBase interface
    virtual void AddPlayerState(APlayerState* PlayerState) override;
    virtual void RemovePlayerState(APlayerState* PlayerState) override;
    virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;
    //~End of AGameStateBase interface

    //~IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~End of IAbilitySystemInterface

public:
    // Gets the ability system component used for game wide things
    UFUNCTION(BlueprintCallable, Category = "Monument Valley|GameState")
    UMvAbilitySystemComponent* GetLyraAbilitySystemComponent() const { return AbilitySystemComponent; }

protected:
    // The ability system component subobject for game-wide things (primarily gameplay cues)
    UPROPERTY(VisibleAnywhere, Category = "Monument Valley|GameState")
    TObjectPtr<UMvAbilitySystemComponent> AbilitySystemComponent;
};
