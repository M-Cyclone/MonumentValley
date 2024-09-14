// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Player/MvPlayerAvatar.h"

#include "Camera/CameraComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/SpringArmComponent.h"

#include "GameCore/Lego/MapHandlerSubsystem.h"
#include "GameCore/Lego/MvBrick.h"
#include "GameCore/Lego/MvBrickMap.h"
#include "GameCore/Lego/MvLegoComponent.h"

#include "GameCore/Player/MvPlayerAvatarController.h"

// Sets default values
AMvPlayerAvatar::AMvPlayerAvatar()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Sprint Arm"));
    SpringArmComponent->SetupAttachment(RootComponent);

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent);
    CameraComponent->ProjectionMode = ECameraProjectionMode::Type::Orthographic;
    CameraComponent->SetOrthoWidth(2000.0f);
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

    GetWorld()->GetSubsystem<UMapHandlerSubsystem>()->AddSetCurrMapCallback(
        FWorldSetCurrMapDelegate::FDelegate::CreateUObject(this, &ThisClass::SetUpCameraPose));
}

// Called every frame
void AMvPlayerAvatar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (const auto MvController = Cast<AMvPlayerAvatarController>(Controller))
    {
        const bool bTriggeredSetControlBrick = MvController->IsTriggeredSetControlBrick();
        const bool bTriggeredSetMoveTarget   = MvController->IsTriggeredSetMoveTarget();

        if (bTriggeredSetControlBrick || bTriggeredSetMoveTarget)
        {
            if (auto Result = GetMouseInteractResult(MvController))
            {
                AActor* HitActor = Result->HitResult.GetActor();

                if (const auto Brick = Cast<AMvBrick>(HitActor))
                {
                    if (const auto Map = Brick->GetOwnerBrickMap())
                    {
                        if (const auto Comp = Map->GetLegoComponent())
                        {
                            if (bTriggeredSetControlBrick)
                            {
                                Comp->ProcessControlBrick(*Result);
                            }

                            if (bTriggeredSetMoveTarget)
                            {
                                Comp->ProcessSetTargetPos(*Result);
                            }
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
    if (const auto MvController = Cast<AMvPlayerAvatarController>(Controller))
    {
        MvController->TriggeredSetControlBrick();
    }
}

void AMvPlayerAvatar::OnSetMoveTarget(const FInputActionValue& Value)
{
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

void AMvPlayerAvatar::SetUpCameraPose(const AMvBrickMap* Map)
{
    static const FRotator Rotation = FRotator(-FMath::Asin(FMath::Sqrt(1.0f / 3.0f)) * 180.0f / PI, -135.0f, 0.0f);

    const int32 VoxelEdgeCount = Map->GetBrickComponent()->GetVoxelEdgeCount();

    const FVector MapLoc          = Map->GetActorLocation();
    const FVector MapCenterOffset = MapLoc + FVector(VoxelEdgeCount * 50.0f, VoxelEdgeCount * 50.0f, 0);
    const FVector CameraLocOffset = FVector(VoxelEdgeCount * 100.0f, VoxelEdgeCount * 100.0f, VoxelEdgeCount * 100.0f);

    const float Distance = CameraLocOffset.Length();

    SetActorLocation(MapLoc + MapCenterOffset);

    SpringArmComponent->SetWorldRotation(Rotation);
    SpringArmComponent->SetWorldLocation(MapLoc + MapCenterOffset);
    SpringArmComponent->TargetArmLength = Distance;

    CameraComponent->SetOrthoWidth(Distance * 1.5f);
}
