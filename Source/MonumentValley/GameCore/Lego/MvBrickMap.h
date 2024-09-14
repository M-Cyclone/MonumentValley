// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "GameCore/Lego/MvBrickComponent.h"
#include "GameCore/Lego/MvLegoComponent.h"

#include "MvBrickMap.generated.h"

class AMvBrick;

USTRUCT(BlueprintType)
struct FBrickAssetInfo
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    uint8 MinX;

    UPROPERTY(EditDefaultsOnly)
    uint8 MaxX;

    UPROPERTY(EditDefaultsOnly)
    uint8 MinY;

    UPROPERTY(EditDefaultsOnly)
    uint8 MaxY;

    UPROPERTY(EditDefaultsOnly)
    uint8 MinZ;

    UPROPERTY(EditDefaultsOnly)
    uint8 MaxZ;
};

UENUM(BlueprintType)
enum class ELegoTransformType : uint8
{
    Translation,
    Rotation
};

UENUM(BlueprintType)
enum class ELegoRotationAxis : uint8
{
    X,
    Y,
    Z
};

USTRUCT(BlueprintType)
struct FLegoAssetInfo
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TArray<FBrickAssetInfo> BrickInfos;

    UPROPERTY(EditDefaultsOnly)
    ELegoTransformType TransformType;

    UPROPERTY(EditDefaultsOnly)
    ELegoRotationAxis RotationAxis;

    UPROPERTY(EditDefaultsOnly)
    int32 AxisX;

    UPROPERTY(EditDefaultsOnly)
    int32 AxisY;
};

UCLASS()
class MONUMENTVALLEY_API AMvBrickMap : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AMvBrickMap();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    void LoadAssetSecondFrame();

protected:
    UPROPERTY(EditDefaultsOnly)
    FName MapAssetName;

public:
    UMvBrickComponent* GetBrickComponent() const { return BrickComp; }
    UMvLegoComponent*  GetLegoComponent() const { return LegoComp; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Monument Valley")
    TObjectPtr<UMvBrickComponent> BrickComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Monument Valley")
    TObjectPtr<UMvLegoComponent> LegoComp;
};
