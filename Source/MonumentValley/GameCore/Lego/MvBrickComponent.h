// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "MvBrickComponent.generated.h"

struct FMouseInteractResult;

class AMvBrick;

UCLASS()
class MONUMENTVALLEY_API UMvBrickComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    UMvBrickComponent();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable)
    bool AddBrick(AMvBrick* NewBrick);

    void CleanAllBricks();

protected:
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<AMvBrick>> Bricks;

public:
    FIntVector GetVoxelLoc(const FVector& Loc) const;

    uint32     GetCompressMapLoc(const FIntVector3& VoxelLoc) const;
    FIntVector GetNearestMapVoxelLocIfValid(const uint32 Loc) const;

    void Construct2DMap();

    void FindPath(const FIntVector3& LocBegin, const FIntVector3& LocEnd, TArray<uint32>& OutPath) const;

    int32 GetVoxelEdgeCount() const { return VoxelEdgeCount; }

    uint32 GetSpawnLocation() const { return SpawnLocation; }

protected:
    int32 VoxelEdgeCount = 0;

    // Marks which 2d point is available.
    TMap<uint32, FIntVector> AxisZ2DMap;

    uint32 SpawnLocation = (uint32)-1;

public:
    void        SetLocationOffset(const FIntVector3& NewLocation) { LocationOffset = NewLocation; }
    FIntVector3 GetLocationOffset() const { return LocationOffset; }

protected:
    FIntVector3 LocationOffset;

public:
    void ProcessSetTargetPos(const FMouseInteractResult& Input);

    void   SetTargetBrick(const AMvBrick* Brick);
    uint32 GetTargetBrick() const { return TargetBrick; }

protected:
    uint32 TargetBrick = (uint32)-1;
};
