// Fill out your copyright notice in the Description page of Project Settings.


#include "MonumentValley/GameCore/Player/MvPlayerController.h"

#include "AbilitySystemGlobals.h"
#include "CommonInputSubsystem.h"
#include "Net/UnrealNetwork.h"

#include "MonumentValley/MvGameplayTags.h"

#include "MvLocalPlayer.h"
#include "MvPlayerState.h"

AMvPlayerController::AMvPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{}

void AMvPlayerController::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AMvPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetActorHiddenInGame(false);
}

void AMvPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AMvPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Disable replicating the PC target view as it doesn't work well for replays or client-side spectating.
    // The engine TargetViewRotation is only set in APlayerController::TickActor if the server knows ahead of time that
    // a specific pawn is being spectated and it only replicates down for COND_OwnerOnly.
    // In client-saved replays, COND_OwnerOnly is never true and the target pawn is not always known at the time of recording.
    // To support client-saved replays, the replication of this was moved to ReplicatedViewRotation and updated in PlayerTick.
    DISABLE_REPLICATED_PROPERTY(APlayerController, TargetViewRotation);
}

void AMvPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    SetIsAutoRunning(false);
}

void AMvPlayerController::OnUnPossess()
{
    // Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
    if (const APawn* PawnBeingUnPossessed = GetPawn())
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
        {
            if (ASC->GetAvatarActor() == PawnBeingUnPossessed)
            {
                ASC->SetAvatarActor(nullptr);
            }
        }
    }

    Super::OnUnPossess();
}

void AMvPlayerController::InitPlayerState()
{
    Super::InitPlayerState();
}

void AMvPlayerController::CleanupPlayerState()
{
    Super::CleanupPlayerState();
}

void AMvPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
}

void AMvPlayerController::ReceivedPlayer()
{
    Super::ReceivedPlayer();
}

void AMvPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
}

void AMvPlayerController::SetPlayer(UPlayer* InPlayer)
{
    Super::SetPlayer(InPlayer);
}

void AMvPlayerController::AddCheats(bool bForce)
{
    Super::AddCheats(bForce);
}

void AMvPlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
    if (bForceFeedbackEnabled)
    {
        if (const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
        {
            const ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
            if (CurrentInputType == ECommonInputType::Gamepad || CurrentInputType == ECommonInputType::Touch)
            {
                InputInterface->SetForceFeedbackChannelValues(ControllerId, ForceFeedbackValues);
                return;
            }
        }
    }

    InputInterface->SetForceFeedbackChannelValues(ControllerId, FForceFeedbackValues());
}

void AMvPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
    Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);
}

void AMvPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
    Super::PreProcessInput(DeltaTime, bGamePaused);
}

void AMvPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
    if (UMvAbilitySystemComponent* Comp = GetMvAbilitySystemComponent())
    {
        Comp->ProcessAbilityInput(DeltaTime, bGamePaused);
    }

    Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AMvPlayerController::SetIsAutoRunning(const bool bEnabled)
{
    const bool bIsAutoRunning = GetIsAutoRunning();
    if (bEnabled != bIsAutoRunning)
    {
        if (!bEnabled)
        {
            OnEndAutoRun();
        }
        else
        {
            OnStartAutoRun();
        }
    }
}

bool AMvPlayerController::GetIsAutoRunning() const
{
    bool bIsAutoRunning = false;
    if (const UMvAbilitySystemComponent* Comp = GetMvAbilitySystemComponent())
    {
        bIsAutoRunning = Comp->GetTagCount(MvGameplayTags::Status_AutoRunning) > 0;
    }
    return bIsAutoRunning;
}

void AMvPlayerController::OnStartAutoRun()
{
    if (UMvAbilitySystemComponent* Comp = GetMvAbilitySystemComponent())
    {
        Comp->SetLooseGameplayTagCount(MvGameplayTags::Status_AutoRunning, 1);
        K2_OnStartAutoRun();
    }
}

void AMvPlayerController::OnEndAutoRun()
{
    if (UMvAbilitySystemComponent* Comp = GetMvAbilitySystemComponent())
    {
        Comp->SetLooseGameplayTagCount(MvGameplayTags::Status_AutoRunning, 0);
        K2_OnEndAutoRun();
    }
}

AMvPlayerState* AMvPlayerController::GetMvPlayerState() const
{
    return CastChecked<AMvPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UMvAbilitySystemComponent* AMvPlayerController::GetMvAbilitySystemComponent() const
{
    const AMvPlayerState* MvPlayerState = GetMvPlayerState();
    return (MvPlayerState ? MvPlayerState->GetMvAbilitySystemComponent() : nullptr);
}
