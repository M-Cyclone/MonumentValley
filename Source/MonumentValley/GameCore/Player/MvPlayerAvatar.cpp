// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Player/MvPlayerAvatar.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
}

// Called to bind functionality to input
void AMvPlayerAvatar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MouseClickAction, ETriggerEvent::Started, this, &ThisClass::OnMouseClick);
    }
}

void AMvPlayerAvatar::OnMouseClick(const FInputActionValue& Value)
{
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        FVector MouseLocation;
        FVector MouseDirection;
        if (!PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
        {
            return;
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
            return;
        }

        DrawDebugLine(GetWorld(), PosBegin, HitResult.Location, FColor::Blue, false, 1.0f);

        DrawDebugSphere(GetWorld(), HitResult.Location, 100.0f, 8, FColor::Red, false, 1.0f);

        DrawDebugBox(GetWorld(), MouseLocation, FVector{ 1.0f, 1.0f, 1.0f }, FColor::Yellow, false, 1.0f);

        DrawDebugCamera(GetWorld(), CameraLocation, CameraRotation, 30.0f, 1.0f, FColor::Green, false, 1.0f);
    }
}
