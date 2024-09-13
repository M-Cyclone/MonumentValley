// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "MvBrickComponent.generated.h"

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

public:
    uint32 GetAxisZ2DMapLoc(const FIntVector3& VoxelLoc) const;

    void Construct2DMap();

    void FindPath(const FIntVector3& LocBegin, const FIntVector3& LocEnd, TArray<uint32>& OutPath) const;

protected:
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<AMvBrick>> Bricks;

    int32 VoxelEdgeCount = 0;

    // Marks which 2d point is available.
    TSet<uint32> AxisZ2DMap;
};
