// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Lego/MvBrickComponent.h"

#include "GameCore/Lego/MvBrick.h"

#include "GameCore/Player/MvPlayerAvatar.h"

// Sets default values
UMvBrickComponent::UMvBrickComponent()
{}

// Called when the game starts or when spawned
void UMvBrickComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool UMvBrickComponent::AddBrick(AMvBrick* NewBrick)
{
    if (!NewBrick)
    {
        return false;
    }

    for (const AMvBrick* Brick : Bricks)
    {
        if (Brick == NewBrick)
        {
            return false;
        }
    }

    Bricks.Push(NewBrick);
    return true;
}

void UMvBrickComponent::CleanAllBricks()
{
    for (TObjectPtr<AMvBrick>& Brick : Bricks)
    {
        Brick->Destroy();
    }
    Bricks.Empty();
}

FIntVector UMvBrickComponent::GetVoxelLoc(const FVector& Loc) const
{
    return FIntVector((int32)(Loc.X * 0.01f - 0.5f), (int32)(Loc.Y * 0.01f - 0.5f), (int32)(Loc.Z * 0.01f - 0.5f)) - LocationOffset;
}

FIntVector UMvBrickComponent::GetNearestMapVoxelLocIfValid(const FIntVector2 ProjLoc) const
{
    const auto It = ProjLocToVoxelLocMap.Find(ProjLoc);
    return It ? (*It)[0] : FIntVector(-1);
}

FIntVector2 UMvBrickComponent::GetProjVoxelLocation(const FIntVector& Loc) const
{
    return FIntVector2(Loc.X + VoxelEdgeCount - 1 - Loc.Z, Loc.Y + VoxelEdgeCount - 1 - Loc.Z);
}

void UMvBrickComponent::Construct2DMap()
{
    if (Bricks.IsEmpty())
    {
        return;
    }

    {
        int32 MaxX = 0;
        int32 MaxY = 0;
        int32 MaxZ = 0;
        for (const AMvBrick* Brick : Bricks)
        {
            const FIntVector Loc = Brick->GetVoxelLocation();
            MaxX                 = FMath::Max(MaxX, Loc.X);
            MaxY                 = FMath::Max(MaxY, Loc.Y);
            MaxZ                 = FMath::Max(MaxZ, Loc.Z);
        }

        VoxelEdgeCount = FMath::Max(FMath::Max(MaxX, MaxY), MaxZ);
    }

    // Used for culling.
    Bricks.Sort(
        [](const TObjectPtr<AMvBrick>& BrickA, const TObjectPtr<AMvBrick>& BrickB)
        {
            const FIntVector LocA = BrickA->GetVoxelLocation();
            const FIntVector LocB = BrickB->GetVoxelLocation();

            return (LocA.X + LocA.Y + LocA.Z) < (LocB.X + LocB.Y + LocB.Z);
        });

    ProjLocToVoxelLocMap.Reset();
    CurrMapLocations.Reset();
    BlockedLocations.Reset();

    // Used for setting visibility.
    TMap<FIntVector2, int32> DepthLeft;
    TMap<FIntVector2, int32> DepthRight;

    auto AddToDepthBuffer = [](TMap<FIntVector2, int32>& Buffer, const FIntVector2& ProjLoc, const int32 Depth)
    {
        if (auto It = Buffer.Find(ProjLoc))
        {
            *It = FMath::Max(*It, Depth);
        }
        else
        {
            Buffer.Add(ProjLoc, Depth);
        }
    };

    for (const AMvBrick* Brick : Bricks)
    {
        const FIntVector  NewLoc  = Brick->GetVoxelLocation();
        const FIntVector2 ProjLoc = GetProjVoxelLocation(NewLoc);

        const int32 NewLayer = NewLoc.X + NewLoc.Y + NewLoc.Z;

        if (const auto It = ProjLocToVoxelLocMap.Find(ProjLoc))
        {
            check(!It->IsEmpty());

            It->Push(NewLoc);

            const int32 OldLayer = (*It)[0].X + (*It)[0].Y + (*It)[0].Z;


            if (NewLayer > OldLayer)
            {
                It->Swap(0, It->Num() - 1);
            }
        }
        else
        {
            ProjLocToVoxelLocMap.Add(ProjLoc, TArray{ NewLoc });
        }

        CurrMapLocations.Add(NewLoc, true);
        BlockedLocations.Add(FIntVector(NewLoc.X, NewLoc.Y, NewLoc.Z - 1));

        AddToDepthBuffer(DepthLeft, ProjLoc, NewLayer);
        AddToDepthBuffer(DepthLeft, ProjLoc + FIntVector2(1, 0), NewLayer);
        AddToDepthBuffer(DepthLeft, ProjLoc + FIntVector2(1, 1), NewLayer);

        AddToDepthBuffer(DepthRight, ProjLoc, NewLayer);
        AddToDepthBuffer(DepthRight, ProjLoc + FIntVector2(0, 1), NewLayer);
        AddToDepthBuffer(DepthRight, ProjLoc + FIntVector2(1, 1), NewLayer);
    }

    for (auto It = CurrMapLocations.begin(); It != CurrMapLocations.end(); ++It)
    {
        const FIntVector2 ProjLoc = GetProjVoxelLocation(It->Key);
        const int32       Layer   = It->Key.X + It->Key.Y + It->Key.Z;

        const auto DepthL = DepthLeft.Find(ProjLoc);
        const auto DepthR = DepthLeft.Find(ProjLoc);

        check(DepthL && DepthR);

        It->Value = (*DepthL == Layer && *DepthR == Layer);
    }

    SpawnLocation = Bricks[0]->GetVoxelLocation();
}

void UMvBrickComponent::FindPath(const FIntVector& LocBegin, const FIntVector& LocEnd, TArray<FIntVector>& OutPath) const
{
    static const FIntVector InvalidLoc = FIntVector(-1);

    OutPath.Reset();

    if (!CurrMapLocations.Find(LocBegin) || !CurrMapLocations.Find(LocEnd))
    {
        return;
    }

    TQueue<FIntVector> PosQueue;
    PosQueue.Enqueue(LocBegin);

    TMap<FIntVector, FIntVector> Parent;
    Parent.Add(LocBegin) = InvalidLoc;

    static const FIntVector2 DirList[] = {
        {-1,  0},
        { 1,  0},
        { 0, -1},
        { 0,  1}
    };

    const int32 MaxPosValue = VoxelEdgeCount * 2;

    while (!PosQueue.IsEmpty())
    {
        FIntVector Curr;
        PosQueue.Dequeue(Curr);

        if (Curr == LocEnd)
        {
            OutPath.Push(Curr);

            while (const auto It = Parent.Find(Curr))
            {
                if (*It == InvalidLoc)
                {
                    return;
                }
                Curr = *It;
                OutPath.Push(Curr);
            }
        }

        const FIntVector2 ProjLoc = GetProjVoxelLocation(Curr);

        const auto ItCurr = CurrMapLocations.Find(Curr);
        check(ItCurr);

        const bool bCurrVisible = *ItCurr;

        for (const auto& Dir : DirList)
        {
            const FIntVector2 NewLoc = ProjLoc + Dir;
            const int32       NewX   = NewLoc.X;
            const int32       NewY   = NewLoc.Y;

            if (NewX < 0 || NewY < 0 || NewX >= MaxPosValue || NewY >= MaxPosValue)
            {
                continue;
            }

            const auto It = ProjLocToVoxelLocMap.Find(NewLoc);
            if (!It)
            {
                continue;
            }

            for (const auto& NewVoxelLoc : *It)
            {
                if (Parent.Find(NewVoxelLoc))
                {
                    continue;
                }

                if (BlockedLocations.Find(NewVoxelLoc))
                {
                    continue;
                }

                const FIntVector Diff = NewVoxelLoc - Curr;
                const int32      Dist = FMath::Abs(Diff.X) + FMath::Abs(Diff.Y) + FMath::Abs(Diff.Z);

                const auto ItNew = CurrMapLocations.Find(NewVoxelLoc);
                check(ItNew);

                const bool bNewVisible = *ItNew;

                if (Dist != 1 && (!(bCurrVisible && bNewVisible)))
                {
                    continue;
                }

                // Check if can be seen. Teleport is enabled only if both brick are visible.

                PosQueue.Enqueue(NewVoxelLoc);
                Parent.Add(NewVoxelLoc) = Curr;
            }
        }
    }
}

void UMvBrickComponent::ProcessSetTargetPos(const FMouseInteractResult& Input)
{
    const auto Brick = Cast<AMvBrick>(Input.HitResult.GetActor());
    check(Brick);

    const FVector_NetQuantizeNormal& Normal = Input.HitResult.Normal;
    if (!FMath::IsNearlyEqual(Normal.Dot(FVector::UpVector), 1.0f))
    {
        // Only care about up normal case.
        return;
    }

    SetTargetBrick(Brick);
}

void UMvBrickComponent::SetTargetBrick(const AMvBrick* Brick)
{
    check(Brick);
    if (const auto It = ProjLocToVoxelLocMap.Find(GetProjVoxelLocation(Brick->GetVoxelLocation())))
    {
        TargetBrick = (*It)[0];
    }
}
