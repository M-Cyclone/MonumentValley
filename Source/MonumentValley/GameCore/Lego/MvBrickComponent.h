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

    FIntVector GetNearestMapVoxelLocIfValid(const FIntVector2 ProjLoc) const;

    FIntVector2 GetProjVoxelLocation(const FIntVector& Loc) const;

    int32 GetVoxelEdgeCount() const { return VoxelEdgeCount; }

    FIntVector GetSpawnLocation() const { return SpawnLocation; }

public:
    void Construct2DMap();

    void FindPath(const FIntVector& LocBegin, const FIntVector& LocEnd, TArray<FIntVector>& OutPath) const;

protected:
    int32 VoxelEdgeCount = 0;

    FIntVector SpawnLocation = FIntVector(-1);

    TMap<FIntVector2, TArray<FIntVector>> ProjLocToVoxelLocMap;

    TMap<FIntVector, bool> CurrMapLocations;  // Maps and the visibility.

    TSet<FIntVector> BlockedLocations;

public:
    void       SetLocationOffset(const FIntVector& NewLocation) { LocationOffset = NewLocation; }
    FIntVector GetLocationOffset() const { return LocationOffset; }

protected:
    FIntVector LocationOffset;

public:
    void ProcessSetTargetPos(const FMouseInteractResult& Input);

    void       SetTargetBrick(const AMvBrick* Brick);
    FIntVector GetTargetBrick() const { return TargetBrick; }

protected:
    FIntVector TargetBrick = FIntVector(-1);
};
