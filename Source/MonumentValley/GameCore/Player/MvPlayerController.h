// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

#include "MonumentValley/GameCore/AbilitySystem/MvAbilitySystemComponent.h"

#include "MvPlayerController.generated.h"

class AMvPlayerState;

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API AMvPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AMvPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
    //~AActor interface
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //~End of AActor interface

    //~AController interface
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    virtual void InitPlayerState() override;
    virtual void CleanupPlayerState() override;
    virtual void OnRep_PlayerState() override;
    //~End of AController interface

    //~APlayerController interface
    virtual void ReceivedPlayer() override;
    virtual void PlayerTick(float DeltaTime) override;
    virtual void SetPlayer(UPlayer* InPlayer) override;
    virtual void AddCheats(bool bForce) override;
    virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
    virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
    virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
    virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
    //~End of APlayerController interface

public:
    UFUNCTION(BlueprintCallable, Category = "Monument Valley|Character")
    void SetIsAutoRunning(const bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Monument Valley|Character")
    bool GetIsAutoRunning() const;

protected:
    void OnStartAutoRun();
    void OnEndAutoRun();

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnStartAutoRun"))
    void K2_OnStartAutoRun();

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnEndAutoRun"))
    void K2_OnEndAutoRun();

public:
    UFUNCTION(BlueprintCallable, Category = "Monument Valley|PlayerController")
    AMvPlayerState* GetMvPlayerState() const;

    UFUNCTION(BlueprintCallable, Category = "Monument Valley|PlayerController")
    UMvAbilitySystemComponent* GetMvAbilitySystemComponent() const;
};
