// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "MvBrick.generated.h"

class UBoxComponent;

class AMvBrickMap;

UCLASS()
class MONUMENTVALLEY_API AMvBrick : public AActor
{
    GENERATED_BODY()

public:
    AMvBrick();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMeshComponent> BrickMeshComp;  // Used to be shown as a Cube.

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UBoxComponent> BoxCollisionComp;  // Used in mouse clicking.

public:
    FIntVector3 GetVoxelLocation() const { return VoxelLocation; }
    void        SetVoxelLocation(const FIntVector3& NewVoxelLoc) { VoxelLocation = NewVoxelLoc; }

protected:
    FIntVector3 VoxelLocation;  // Voxel location in current BrickMap.

public:
    UFUNCTION(BlueprintCallable)
    void SetOwnerBrickMap(AMvBrickMap* NewOwner);

    UFUNCTION(BlueprintCallable)
    AMvBrickMap* GetOwnerBrickMap() const { return OwnerBrickMap; }

protected:
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AMvBrickMap> OwnerBrickMap;
};
