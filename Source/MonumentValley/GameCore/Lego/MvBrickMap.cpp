// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Lego/MvBrickMap.h"

#include "Engine/AssetManager.h"
#include "MapHandlerSubsystem.h"
#include "Misc/DataValidation.h"

#include "GameCore/Lego/MvBrickMapAsset.h"

// Sets default values
AMvBrickMap::AMvBrickMap()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    BrickComp = CreateDefaultSubobject<UMvBrickComponent>(TEXT("Brick Comp"));
    LegoComp  = CreateDefaultSubobject<UMvLegoComponent>(TEXT("Lego Comp"));
}

// Called when the game starts or when spawned
void AMvBrickMap::BeginPlay()
{
    Super::BeginPlay();

    // We shift the map by Global voxel location.
    SetActorLocation(FVector::Zero());

    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::LoadAssetSecondFrame);
}

void AMvBrickMap::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bIsMapLoaded)
    {
        BrickComp->CleanAllBricks();
        bIsMapLoaded = false;
    }

    Super::EndPlay(EndPlayReason);
}

// Called every frame
void AMvBrickMap::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMvBrickMap::LoadAssetSecondFrame()
{
    const FPrimaryAssetId MapAssetId = FPrimaryAssetId(FPrimaryAssetType("MvBrickMapAsset"), MapAssetName);
    if (MapAssetId.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("[AMvBrickMap] Start Load LegoMap (%s)."), *MapAssetId.ToString());

        const FSoftObjectPath               MapAssetPath = UAssetManager::Get().GetPrimaryAssetPath(MapAssetId);
        const TSubclassOf<UMvBrickMapAsset> AssetClass   = Cast<UClass>(MapAssetPath.TryLoad());
        const UMvBrickMapAsset*             Map          = GetDefault<UMvBrickMapAsset>(AssetClass);

        {
            FDataValidationContext Context;
            check(Map->IsDataValid(Context) == EDataValidationResult::Valid);
        }

        if (UClass* BrickClass = Map->BrickClass)
        {
            // We move the bricks to the map location, and we will not rotate them because they are expected as axis-aligned.
            const FIntVector3 VoxelLocation = GetLocationOffset();
            const FVector     CurrLocation  = FVector(VoxelLocation.X, VoxelLocation.Y, VoxelLocation.Z) * 100.0f;

            for (const auto& [MinX, MaxX, MinY, MaxY, MinZ, MaxZ] : Map->StaticBrickInfos)
            {
                for (uint32 Z = MinZ; Z <= MaxZ; ++Z)
                {
                    for (uint32 Y = MinY; Y <= MaxY; ++Y)
                    {
                        for (uint32 X = MinX; X <= MaxX; ++X)
                        {
                            const FVector  Location = FVector(X, Y, Z) * 100.0f + CurrLocation;
                            const FRotator Rotation = FRotator::ZeroRotator;

                            FActorSpawnParameters SpawnParameters{};
                            SpawnParameters.Owner = this;

                            auto Brick = Cast<AMvBrick>(GetWorld()->SpawnActor(BrickClass, &Location, &Rotation, SpawnParameters));
                            check(Brick);

                            Brick->SetOwnerBrickMap(this);
                            Brick->SetVoxelLocation(FIntVector3(X, Y, Z));

                            BrickComp->AddBrick(Brick);
                        }
                    }
                }
            }

            BrickComp->Construct2DMap();

            GetWorld()->GetSubsystem<UMapHandlerSubsystem>()->AddMap(this);

            bIsMapLoaded = true;
        }

        UE_LOG(LogTemp, Log, TEXT("[AMvBrickMap] Finish Load LegoMap (%s) Name (%s)."), *MapAssetId.ToString(), *Map->GetName());
    }
}
