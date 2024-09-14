// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Lego/MapHandlerSubsystem.h"

#include "GameCore/Lego/MvBrickMap.h"

bool UMapHandlerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return Super::ShouldCreateSubsystem(Outer);
}

void UMapHandlerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UMapHandlerSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UMapHandlerSubsystem::AddMap(AMvBrickMap* NewMap)
{
    for (const TObjectPtr<AMvBrickMap>& Map : Maps)
    {
        if (Map.Get() == NewMap)
        {
            return;
        }
    }

    Maps.Add(NewMap);
    if (!CurrMap)
    {
        CurrMap = NewMap;
        OnSetCurrMap.Broadcast(CurrMap);
    }
}

void UMapHandlerSubsystem::SetCurrMap(const FString& Target)
{
    for (const TObjectPtr<AMvBrickMap>& Map : Maps)
    {
        const FString& Name = Map->GetName();
        if (Name == Target)
        {
            CurrMap = Map;
            OnSetCurrMap.Broadcast(CurrMap);
            return;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[UMapHandlerSubsystem] SetCurrMap failed with Target (%s)."), *Target);
}

void UMapHandlerSubsystem::AddSetCurrMapCallback(FWorldSetCurrMapDelegate::FDelegate&& Func)
{
    OnSetCurrMap.Add(MoveTemp(Func));
}
