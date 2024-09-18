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
        const FIntVector NewTarget = BrickComp->GetTargetBrick();
        if (NewTarget != FIntVector(-1) && NewTarget != TargetBrick)
        {
            const FIntVector2 MapSpaceLoc = FIntVector2((int32)MapSpaceLoc2D.X, (int32)MapSpaceLoc2D.Y);

            BrickComp->FindPath(BrickComp->GetNearestMapVoxelLocIfValid(MapSpaceLoc),
                                BrickComp->GetNearestMapVoxelLocIfValid(BrickComp->GetProjVoxelLocation(NewTarget)),
                                PathToTarget);

            CurrPathIndex = PathToTarget.Num() - 1;
            TargetBrick   = NewTarget;
        }
    }

    if (PathToTarget.IsEmpty())
    {
        return;
    }

    if (CurrPathIndex < 0 || CurrPathIndex >= PathToTarget.Num())
    {
        return;
    }

    const float InvVelocity = 1.0f / MoveVelocity;

    while (DeltaTime > FLT_EPSILON)
    {
        if (CurrPathIndex > 0)
        {
            // We have not reach the target brick, need to find a way.

            const FIntVector NextVoxelLoc = PathToTarget[CurrPathIndex - 1];
            const FVector2f  Next2dLoc    = VoxelLocToMapLoc2d(NextVoxelLoc, BrickComp->GetVoxelEdgeCount());

            const FVector2f DeltaMove2d = Next2dLoc - MapSpaceLoc2D;
            const float     AbsDeltaX   = FMath::Abs(DeltaMove2d.X);
            const float     AbsDeltaY   = FMath::Abs(DeltaMove2d.Y);

            if (AbsDeltaX > FLT_EPSILON && AbsDeltaY > FLT_EPSILON)
            {
                // Consider this can be easy to figure out the rotation.

                // Curr grid center.
                const FIntVector CurrVoxelLoc = PathToTarget[CurrPathIndex];
                const FVector2f  Curr2dLoc    = VoxelLocToMapLoc2d(CurrVoxelLoc, BrickComp->GetVoxelEdgeCount());

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
                    // Move directly if we have enough time.

                    MapSpaceLoc2D = Next2dLoc;
                    DeltaTime -= MoveTimeCost;

                    CurrPathIndex -= 1;
                }
                else
                {
                    // Just move until the time ends.

                    const float MaxMove = DeltaTime * MoveVelocity;

                    const float ParamX = AbsDeltaX > FLT_EPSILON ? (DeltaMove2d.X > 0 ? 1.0f : -1.0f) : 0.0f;
                    const float ParamY = AbsDeltaY > FLT_EPSILON ? (DeltaMove2d.Y > 0 ? 1.0f : -1.0f) : 0.0f;

                    MapSpaceLoc2D += FVector2f(MaxMove, 0.0f) * ParamX + FVector2f(0.0f, MaxMove) * ParamY;

                    const FIntVector2 MapSpaceLoc = FIntVector2((int32)MapSpaceLoc2D.X, (int32)MapSpaceLoc2D.Y);
                    const FIntVector2 CurrLoc     = BrickComp->GetProjVoxelLocation(PathToTarget[CurrPathIndex]);

                    if (MapSpaceLoc != CurrLoc)
                    {
                        // Move to next grid in the path.
                        CurrPathIndex -= 1;
                    }

                    DeltaTime = 0.0f;
                }
            }
        }
        else
        {
            // We have been in the target grid.
            const FIntVector TargetVoxelLoc = PathToTarget[CurrPathIndex];
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
    check(!PathToTarget.IsEmpty());

    const FIntVector NearVoxelLoc = [this]
    {
        const FIntVector PrevVoxelLoc = PathToTarget[FMath::Min(CurrPathIndex + 1, PathToTarget.Num() - 1)];
        const FIntVector NextVoxelLoc = PathToTarget[FMath::Max(CurrPathIndex - 1, 0)];

        const FIntVector2 PrevProjLoc = BrickComp->GetProjVoxelLocation(PrevVoxelLoc);
        const FIntVector2 NextProjLoc = BrickComp->GetProjVoxelLocation(NextVoxelLoc);

        const FVector2f PrevProjLocDist = FVector2f(PrevProjLoc.X, PrevProjLoc.Y) - MapSpaceLoc2D;
        const FVector2f NextProjLocDist = FVector2f(NextProjLoc.X, NextProjLoc.Y) - MapSpaceLoc2D;

        const float PrevDist = FMath::Abs(PrevProjLocDist.X) + FMath::Abs(PrevProjLocDist.Y);
        const float NextDist = FMath::Abs(NextProjLocDist.X) + FMath::Abs(NextProjLocDist.Y);

        return PrevDist < NextDist ? PrevVoxelLoc : NextVoxelLoc;
    }();

    const FIntVector CurrVoxelLoc = PathToTarget[CurrPathIndex];

    const int32 LayerNear = NearVoxelLoc.X + NearVoxelLoc.Y + NearVoxelLoc.Z;
    const int32 LayerCurr = CurrVoxelLoc.X + CurrVoxelLoc.Y + CurrVoxelLoc.Z;

    const FIntVector MapVoxelLoc = LayerNear > LayerCurr ? NearVoxelLoc : CurrVoxelLoc;
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

    const FIntVector SpawnLocVoxel = BrickComp->GetNearestMapVoxelLocIfValid(BrickComp->GetProjVoxelLocation(TargetBrick));

    MapSpaceLoc2D = VoxelLocToMapLoc2d(SpawnLocVoxel, BrickComp->GetVoxelEdgeCount());

    const FVector  BornLoc = GetOwner()->GetActorLocation();
    const FRotator BornRot = GetOwner()->GetActorRotation();

    const FVector NewLocation = (FVector(SpawnLocVoxel) + FVector(0.5f, 0.5f, 1.0f)) * 100.0f;
    MoveUpdatedComponent(NewLocation - BornLoc, BornRot, false);
}
