// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Character/MvMovementComponent.h"

#include "GameCore/Lego/MapHandlerSubsystem.h"
#include "GameCore/Lego/MvBrickComponent.h"
#include "GameCore/Lego/MvBrickMap.h"

namespace
{
FVector2f VoxelLocToMapLoc2d(const FIntVector& VoxelLoc, const int32 EdgeCount)
{
    return FVector2f(VoxelLoc.X, VoxelLoc.Y) + FVector2f(EdgeCount - 1.0f - VoxelLoc.Z) + FVector2f(0.5f, 0.5f);
}
}  // namespace

// Sets default values for this component's properties
UMvMovementComponent::UMvMovementComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UMvMovementComponent::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetSubsystem<UMapHandlerSubsystem>()->AddSetCurrMapCallback(
        FWorldSetCurrMapDelegate::FDelegate::CreateUObject(this, &ThisClass::OnSetCurrMap));
}

// Called every frame
void UMvMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!BrickComp)
    {
        return;
    }

    // Update target brick if changed.
    {
        const uint32 NewTarget = BrickComp->GetTargetBrick();
        if (NewTarget != (uint32)-1 && NewTarget != TargetBrick)
        {
            const int32  MapSpaceLocX        = (int32)(MapSpaceLoc2D.X - 0.5f);
            const int32  MapSpaceLocY        = (int32)(MapSpaceLoc2D.Y - 0.5f);
            const uint32 MapSpaceCompressLoc = (((uint32)MapSpaceLocX) << 16) | ((uint32)MapSpaceLocY);

            BrickComp->FindPath(BrickComp->GetNearestMapVoxelLocIfValid(MapSpaceCompressLoc),
                                BrickComp->GetNearestMapVoxelLocIfValid(NewTarget),
                                PathToTarget);

            CurrPathIndex = PathToTarget.Num() - 1;
            TargetBrick   = NewTarget;
        }
    }

    // MoveUpdatedComponent()

    const float InvVelocity = 1.0f / MoveVelocity;

    if (CurrPathIndex < 0 || CurrPathIndex >= PathToTarget.Num())
    {
        return;
    }

    while (DeltaTime > FLT_EPSILON)
    {
        if (CurrPathIndex > 0)
        {
            const uint32     NextCompressLoc = PathToTarget[CurrPathIndex - 1];
            const FIntVector NextVoxelLoc    = BrickComp->GetNearestMapVoxelLocIfValid(NextCompressLoc);
            const FVector2f  Next2dLoc       = VoxelLocToMapLoc2d(NextVoxelLoc, BrickComp->GetVoxelEdgeCount());

            const FVector2f DeltaMove2d = Next2dLoc - MapSpaceLoc2D;
            const float     AbsDeltaX   = FMath::Abs(DeltaMove2d.X);
            const float     AbsDeltaY   = FMath::Abs(DeltaMove2d.Y);

            if (AbsDeltaX > FLT_EPSILON && AbsDeltaY > FLT_EPSILON)
            {
                // Curr grid center.
                const uint32     CurrCompressLoc = PathToTarget[CurrPathIndex];
                const FIntVector CurrVoxelLoc    = BrickComp->GetNearestMapVoxelLocIfValid(CurrCompressLoc);
                const FVector2f  Curr2dLoc       = VoxelLocToMapLoc2d(CurrVoxelLoc, BrickComp->GetVoxelEdgeCount());

                const FVector2f InGridDeltaMove2d = Curr2dLoc - MapSpaceLoc2D;
                const float     InGridAbsDeltaX   = FMath::Abs(InGridDeltaMove2d.X);
                const float     InGridAbsDeltaY   = FMath::Abs(InGridDeltaMove2d.Y);

                check(FMath::IsNearlyZero(InGridAbsDeltaX * InGridAbsDeltaY));

                const float MoveDist2d   = InGridAbsDeltaX + InGridAbsDeltaY;
                const float MoveTimeCost = MoveDist2d * InvVelocity;

                if (MoveTimeCost <= DeltaTime)
                {
                    MapSpaceLoc2D = Curr2dLoc;
                    DeltaTime -= MoveTimeCost;
                }
                else
                {
                    const float MaxMove = DeltaTime * MoveVelocity;

                    const float ParamX = InGridAbsDeltaX > FLT_EPSILON ? (InGridDeltaMove2d.X > 0 ? 1.0f : -1.0f) : 0.0f;
                    const float ParamY = InGridAbsDeltaY > FLT_EPSILON ? (InGridDeltaMove2d.Y > 0 ? 1.0f : -1.0f) : 0.0f;

                    MapSpaceLoc2D += FVector2f(MaxMove, 0.0f) * ParamX + FVector2f(0.0f, MaxMove) * ParamY;

                    DeltaTime = 0.0f;
                }
            }
            else
            {
                const float MoveDist2d   = AbsDeltaX + AbsDeltaY;
                const float MoveTimeCost = MoveDist2d * InvVelocity;

                if (MoveTimeCost <= DeltaTime)
                {
                    MapSpaceLoc2D = Next2dLoc;
                    DeltaTime -= MoveTimeCost;

                    CurrPathIndex -= 1;
                }
                else
                {
                    const float MaxMove = DeltaTime * MoveVelocity;

                    const float ParamX = AbsDeltaX > FLT_EPSILON ? (DeltaMove2d.X > 0 ? 1.0f : -1.0f) : 0.0f;
                    const float ParamY = AbsDeltaY > FLT_EPSILON ? (DeltaMove2d.Y > 0 ? 1.0f : -1.0f) : 0.0f;

                    MapSpaceLoc2D += FVector2f(MaxMove, 0.0f) * ParamX + FVector2f(0.0f, MaxMove) * ParamY;

                    const int32  MapSpaceLocX        = (int32)(MapSpaceLoc2D.X - 0.5f);
                    const int32  MapSpaceLocY        = (int32)(MapSpaceLoc2D.Y - 0.5f);
                    const uint32 MapSpaceCompressLoc = (((uint32)MapSpaceLocX) << 16) | ((uint32)MapSpaceLocY);
                    if (MapSpaceCompressLoc != PathToTarget[CurrPathIndex])
                    {
                        CurrPathIndex -= 1;
                    }

                    DeltaTime = 0.0f;
                }
            }
        }
        else
        {
            // We have been in the target grid.
            const uint32 TargetCompressLoc = PathToTarget[CurrPathIndex];
            check(TargetCompressLoc == TargetBrick);

            const FIntVector TargetVoxelLoc = BrickComp->GetNearestMapVoxelLocIfValid(TargetCompressLoc);
            const FVector2f  Target2dLoc    = VoxelLocToMapLoc2d(TargetVoxelLoc, BrickComp->GetVoxelEdgeCount());

            const FVector2f DeltaMove2d = Target2dLoc - MapSpaceLoc2D;
            const float     AbsDeltaX   = FMath::Abs(DeltaMove2d.X);
            const float     AbsDeltaY   = FMath::Abs(DeltaMove2d.Y);

            check(FMath::IsNearlyZero(AbsDeltaX * AbsDeltaY));

            const float MoveDist2d   = AbsDeltaX + AbsDeltaY;
            const float MoveTimeCost = MoveDist2d * InvVelocity;

            if (MoveTimeCost <= DeltaTime)
            {
                MapSpaceLoc2D = Target2dLoc;
                DeltaTime -= MoveTimeCost;
                break;
            }
            else
            {
                const float MaxMove = DeltaTime * MoveVelocity;

                const float ParamX = AbsDeltaX > FLT_EPSILON ? (DeltaMove2d.X > 0 ? 1.0f : -1.0f) : 0.0f;
                const float ParamY = AbsDeltaY > FLT_EPSILON ? (DeltaMove2d.Y > 0 ? 1.0f : -1.0f) : 0.0f;

                MapSpaceLoc2D += FVector2f(MaxMove, 0.0f) * ParamX + FVector2f(0.0f, MaxMove) * ParamY;

                DeltaTime = 0.0f;
            }
        }
    }

    // Now we get the MapSpaceLoc2D, we need to inv-proj to get the 3d loc.
    const int32  MapSpaceLocX        = (int32)(MapSpaceLoc2D.X - 0.5f);
    const int32  MapSpaceLocY        = (int32)(MapSpaceLoc2D.Y - 0.5f);
    const uint32 MapSpaceCompressLoc = (((uint32)MapSpaceLocX) << 16) | ((uint32)MapSpaceLocY);

    const FIntVector MapVoxelLoc = BrickComp->GetNearestMapVoxelLocIfValid(MapSpaceCompressLoc);
    const int32      Param       = MapVoxelLoc.Z + 1 - BrickComp->GetVoxelEdgeCount();

    const FVector NewLocation =
        (FVector(MapSpaceLoc2D.X + Param, MapSpaceLoc2D.Y + Param, MapVoxelLoc.Z) + FVector(0.0f, 0.0f, 1.0f)) * 100.0f;
    MoveUpdatedComponent(NewLocation - GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), false);
}

void UMvMovementComponent::OnSetCurrMap(UMvBrickComponent* Comp)
{
    check(Comp);

    BrickComp = Comp;

    TargetBrick = BrickComp->GetSpawnLocation();

    const FIntVector SpawnLocVoxel = BrickComp->GetNearestMapVoxelLocIfValid(TargetBrick);

    MapSpaceLoc2D = VoxelLocToMapLoc2d(SpawnLocVoxel, BrickComp->GetVoxelEdgeCount());

    const FVector  BornLoc = GetOwner()->GetActorLocation();
    const FRotator BornRot = GetOwner()->GetActorRotation();

    const FVector NewLocation = (FVector(SpawnLocVoxel) + FVector(0.5f, 0.5f, 1.0f)) * 100.0f;
    MoveUpdatedComponent(NewLocation - BornLoc, BornRot, false);
}
