// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"

#include "MonumentValley/GameCore/AbilitySystem/MvAbilitySystemComponent.h"

#include "MvPlayerState.generated.h"

class AMvPlayerController;

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API AMvPlayerState
    : public APlayerState
    , public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AMvPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "Lyra|PlayerState")
    AMvPlayerController* GetLyraPlayerController() const;

    UFUNCTION(BlueprintCallable, Category = "Lyra|PlayerState")
    UMvAbilitySystemComponent*       GetMvAbilitySystemComponent() const { return AbilitySystemComponent; }
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    // template <class T>
    // const T* GetPawnData() const
    // {
    //     return Cast<T>(PawnData);
    // }

    // void SetPawnData(const ULyraPawnData* InPawnData);

    //~AActor interface
    virtual void PreInitializeComponents() override;
    virtual void PostInitializeComponents() override;
    //~End of AActor interface

    //~APlayerState interface
    virtual void Reset() override;
    virtual void ClientInitialize(AController* C) override;
    virtual void CopyProperties(APlayerState* PlayerState) override;
    virtual void OnDeactivated() override;
    virtual void OnReactivated() override;
    //~End of APlayerState interface

protected:
    // UPROPERTY(ReplicatedUsing = OnRep_PawnData)
    // TObjectPtr<const ULyraPawnData> PawnData;

private:
    // The ability system component sub-object used by player characters.
    UPROPERTY(VisibleAnywhere, Category = "Lyra|PlayerState")
    TObjectPtr<UMvAbilitySystemComponent> AbilitySystemComponent;
};
