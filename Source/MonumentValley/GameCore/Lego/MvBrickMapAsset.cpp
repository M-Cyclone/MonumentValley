// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Lego/MvBrickMapAsset.h"

#include "Misc/DataValidation.h"

#include "GameCore/Lego/MvBrickMap.h"

#define LOCTEXT_NAMESPACE "UMvBrickMapAsset"

UMvBrickMapAsset::UMvBrickMapAsset()
{}

#if WITH_EDITOR
EDataValidationResult UMvBrickMapAsset::IsDataValid(FDataValidationContext& Context) const
{
    EDataValidationResult Result = CombineDataValidationResults(EDataValidationResult::Valid, Super::IsDataValid(Context));

    TSet<uint32> VisitedGrid;
    for (const auto& Info : StaticBrickInfos)
    {
        for (uint32 Z = Info.MinZ; Z <= Info.MaxZ; ++Z)
        {
            for (uint32 Y = Info.MinY; Y <= Info.MaxY; ++Y)
            {
                for (uint32 X = Info.MinX; X <= Info.MaxX; ++X)
                {
                    const uint32 GridLoc = X | (Y << 8) | (Z << 16);
                    VisitedGrid.Add(GridLoc);
                }
            }
        }
    }

    for (const auto& LegoInfo : LegoBrickInfos)
    {
        for (const auto& [MinX, MaxX, MinY, MaxY, MinZ, MaxZ] : LegoInfo.BrickInfos)
        {
            for (uint32 Z = MinZ; Z <= MaxZ; ++Z)
            {
                for (uint32 Y = MinY; Y <= MaxY; ++Y)
                {
                    for (uint32 X = MinX; X <= MaxX; ++X)
                    {
                        const uint32 GridLoc = X | (Y << 8) | (Z << 16);

                        if (VisitedGrid.Find(GridLoc))
                        {
                            Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
                            Context.AddError(FText::Format(LOCTEXT("UMvBrickMapAsset", "Asset invalid at (%u, %u, %u)."), X, Y, Z));
                        }
                    }
                }
            }
        }
    }

    return Result;
}
#endif

#if WITH_EDITORONLY_DATA
void UMvBrickMapAsset::UpdateAssetBundleData()
{
    Super::UpdateAssetBundleData();
}
#endif

#if WITH_EDITOR
void UMvBrickMapAsset::MakeValid()
{
    TSet<uint32> VisitedGrid;
    for (const auto& Info : StaticBrickInfos)
    {
        for (uint32 Z = Info.MinZ; Z <= Info.MaxZ; ++Z)
        {
            for (uint32 Y = Info.MinY; Y <= Info.MaxY; ++Y)
            {
                for (uint32 X = Info.MinX; X <= Info.MaxX; ++X)
                {
                    const uint32 GridLoc = X | (Y << 8) | (Z << 16);
                    VisitedGrid.Add(GridLoc);
                }
            }
        }
    }

    TArray<FLegoAssetInfo> ValidLegoInfos;
    for (const auto& LegoInfo : LegoBrickInfos)
    {
        bool bLegoValid = true;
        for (const auto& [MinX, MaxX, MinY, MaxY, MinZ, MaxZ] : LegoInfo.BrickInfos)
        {
            for (uint32 Z = MinZ; bLegoValid && Z <= MaxZ; ++Z)
            {
                for (uint32 Y = MinY; bLegoValid && Y <= MaxY; ++Y)
                {
                    for (uint32 X = MinX; bLegoValid && X <= MaxX; ++X)
                    {
                        const uint32 GridLoc = X | (Y << 8) | (Z << 16);

                        if (VisitedGrid.Find(GridLoc))
                        {
                            bLegoValid = false;
                        }
                    }
                }
            }
        }

        if (bLegoValid)
        {
            ValidLegoInfos.Push(LegoInfo);
        }
    }

    LegoBrickInfos = std::move(ValidLegoInfos);
}
#endif
