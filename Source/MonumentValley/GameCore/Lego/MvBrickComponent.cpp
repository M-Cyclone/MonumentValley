// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Lego/MvBrickComponent.h"

#include "GameCore/Lego/MvBrick.h"

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

uint32 UMvBrickComponent::GetAxisZ2DMapLoc(const FIntVector3& VoxelLoc) const
{
    const uint32 A = (uint32)VoxelLoc.X;
    const uint32 B = (uint32)VoxelLoc.Y;
    const uint32 C = (uint32)VoxelEdgeCount - 1 - (uint32)VoxelLoc.Z;

    return ((A + C) << 16) | (B + C);
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
            const FIntVector3 Loc = Brick->GetVoxelLocation();
            MaxX                  = FMath::Max(MaxX, Loc.X);
            MaxY                  = FMath::Max(MaxY, Loc.Y);
            MaxZ                  = FMath::Max(MaxZ, Loc.Z);
        }

        VoxelEdgeCount = FMath::Max(FMath::Max(MaxX, MaxY), MaxZ);
    }

    // Used for culling.
    Bricks.Sort(
        [](const TObjectPtr<AMvBrick>& BrickA, const TObjectPtr<AMvBrick>& BrickB)
        {
            const FIntVector3 LocA = BrickA->GetVoxelLocation();
            const FIntVector3 LocB = BrickB->GetVoxelLocation();

            return (LocA.X + LocA.Y + LocA.Z) < (LocB.X + LocB.Y + LocB.Z);
        });

    AxisZ2DMap.Reset();

    for (const AMvBrick* Brick : Bricks)
    {
        AxisZ2DMap.Add(GetAxisZ2DMapLoc(Brick->GetVoxelLocation()));
    }
}

void UMvBrickComponent::FindPath(const FIntVector3& LocBegin, const FIntVector3& LocEnd, TArray<uint32>& OutPath) const
{
    OutPath.Reset();

    const uint32 Source = GetAxisZ2DMapLoc(LocBegin);
    const uint32 Target = GetAxisZ2DMapLoc(LocEnd);

    if (!AxisZ2DMap.Find(Source) || !AxisZ2DMap.Find(Target))
    {
        return;
    }

    TQueue<uint32> PosQueue;
    PosQueue.Enqueue(Source);

    TMap<uint32, uint32> Parent;
    Parent.Add(Source) = (uint32)-1;

    static constexpr int32 DirList[][2] = {
        {-1,  0},
        { 1,  0},
        { 0, -1},
        { 0,  1}
    };

    const int32 MaxPosValue = VoxelEdgeCount * 2;

    while (!PosQueue.IsEmpty())
    {
        uint32 Curr;
        PosQueue.Dequeue(Curr);

        if (Curr == Target)
        {
            OutPath.Push(Curr);

            while (const auto It = Parent.Find(Curr))
            {
                if (*It == -1)
                {
                    return;
                }
                Curr = *It;
                OutPath.Push(Curr);
            }
        }

        const int32 PosX = (int32)(Curr >> 16);
        const int32 PosY = (int32)(Curr & 0xFFFF);

        for (const auto& Dir : DirList)
        {
            const int32 NewX = PosX + Dir[0];
            const int32 NewY = PosY + Dir[1];

            if (NewX < 0 || NewY < 0 || NewX >= MaxPosValue || NewY >= MaxPosValue)
            {
                continue;
            }

            const uint32 NewKey = (((uint32)NewX) << 16) | ((uint32)NewY);

            const auto It = AxisZ2DMap.Find(NewKey);
            if (!It)
            {
                continue;
            }

            if (Parent.Find(NewKey))
            {
                continue;
            }

            PosQueue.Enqueue(*It);
            Parent.Add(NewKey) = Curr;
        }
    }
}
