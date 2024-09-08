// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Lego/MvBrick.h"

#include "Components/BoxComponent.h"

#include "GameCore/Lego/MvBrickMap.h"

AMvBrick::AMvBrick()
{
    PrimaryActorTick.bCanEverTick = true;

    BrickMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube Mesh Comp"));
    RootComponent = BrickMeshComp;

    BoxCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision Comp"));
    BoxCollisionComp->SetupAttachment(BrickMeshComp);
}

void AMvBrick::BeginPlay()
{
    Super::BeginPlay();
}

void AMvBrick::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMvBrick::SetOwnerBrickMap(AMvBrickMap* NewOwner)
{
    OwnerBrickMap = NewOwner;
}
