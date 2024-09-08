// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"

#include "GameCore/Lego/MvBrick.h"

#include "MvBrickMapAsset.generated.h"

struct FBrickAssetInfo;
struct FLegoAssetInfo;

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API UMvBrickMapAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UMvBrickMapAsset();

public:
#if WITH_EDITOR
    //~UObject interface
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
    //~End of UObject interface
#endif

#if WITH_EDITORONLY_DATA
    //~UPrimaryDataAsset interface
    virtual void UpdateAssetBundleData() override;
    //~End of UPrimaryDataAsset interface
#endif

public:
#if WITH_EDITOR
    void MakeValid();
#endif

public:
    UPROPERTY(EditDefaultsOnly, Category = "BrickMap")
    TSubclassOf<AMvBrick> BrickClass;

    UPROPERTY(EditDefaultsOnly, Category = "BrickMap")
    TArray<FBrickAssetInfo> StaticBrickInfos;

    UPROPERTY(EditDefaultsOnly, Category = "BrickMap")
    TArray<FLegoAssetInfo> LegoBrickInfos;
};
