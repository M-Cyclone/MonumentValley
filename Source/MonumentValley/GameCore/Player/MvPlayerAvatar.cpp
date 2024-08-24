// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Player/MvPlayerAvatar.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MvPlayerState.h"

#include "GameCore/Player/MvPlayerAvatarController.h"

// Sets default values
AMvPlayerAvatar::AMvPlayerAvatar()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMvPlayerAvatar::BeginPlay()
{
    Super::BeginPlay();

    if (const ULocalPlayer* LocalPlayer = (GEngine && GetWorld()) ? GEngine->GetFirstGamePlayer(GetWorld()) : nullptr)
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
        {
            if (MappingContext)
            {
                Subsystem->AddMappingContext(MappingContext, 0);
            }
        }
    }
}

// Called every frame
void AMvPlayerAvatar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (const auto MvController = Cast<AMvPlayerAvatarController>(Controller))
    {
        if (auto MvPlayerState = MvController->GetMvPlayerState())
        {
            const bool bTriggeredSetControlBrick = MvController->IsTriggeredSetControlBrick();
            const bool bTriggeredSetMoveTarget   = MvController->IsTriggeredSetMoveTarget();

            if (bTriggeredSetControlBrick || bTriggeredSetMoveTarget)
            {
                if (auto Result = GetMouseInteractResult(MvController))
                {
                    AActor* HitActor = Result->HitResult.GetActor();

                    if (bTriggeredSetControlBrick)
                    {
                        if (HitActor->Implements<UMvControllableBrick>())
                        {
                            IMvControllableBrick* const ControllableBrick = Cast<IMvControllableBrick>(HitActor);
                            MvPlayerState->Possess(ControllableBrick);
                        }
                    }

                    if (bTriggeredSetMoveTarget)
                    {
                        if (MvPlayerState->IsPossessingBrick())
                        {
                            MvPlayerState->UnPossess();
                        }
                        else
                        {
                            // TODO: Set AIController's target location to HitLocation.
                            UE_LOG(LogTemp, Log, TEXT("World Location (%s)."), *Result->HitResult.Location.ToString());
                        }
                    }
                }
            }
        }
    }
}

// Called to bind functionality to input
void AMvPlayerAvatar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(SetControlBrickAction, ETriggerEvent::Triggered, this, &ThisClass::OnSetControlBrick);
        EnhancedInputComponent->BindAction(SetMoveTargetAction, ETriggerEvent::Completed, this, &ThisClass::OnSetMoveTarget);
    }
}

void AMvPlayerAvatar::OnSetControlBrick(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Log, TEXT("OnSetControlBrick"));

    if (const auto MvController = Cast<AMvPlayerAvatarController>(Controller))
    {
        MvController->TriggeredSetControlBrick();
    }
}

void AMvPlayerAvatar::OnSetMoveTarget(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Log, TEXT("OnSetMoveTarget"));

    if (const auto MvController = Cast<AMvPlayerAvatarController>(Controller))
    {
        MvController->TriggeredSetMoveTarget();
    }
}

auto AMvPlayerAvatar::GetMouseInteractResult(const APlayerController* PlayerController) const -> TOptional<FMouseInteractResult>
{
    if (!PlayerController)
    {
        return {};
    }

    FVector MouseLocation;
    FVector MouseDirection;
    if (!PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
    {
        return {};
    }

    FVector  CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    const FVector PosBegin = CameraLocation;
    const FVector PosEnd   = CameraLocation + MouseDirection * 10000.0f;

    FCollisionObjectQueryParams CollisionQueryParams;
    CollisionQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
    CollisionQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

    FHitResult HitResult;
    if (!GetWorld()->LineTraceSingleByObjectType(HitResult, PosBegin, PosEnd, CollisionQueryParams))
    {
        return {};
    }

    return FMouseInteractResult{ HitResult, MouseLocation, MouseDirection, CameraLocation, CameraRotation };
}
