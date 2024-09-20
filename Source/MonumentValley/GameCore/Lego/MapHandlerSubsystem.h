// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Subsystems/WorldSubsystem.h"

#include "MapHandlerSubsystem.generated.h"


class UMvBrickComponent;
class AMvBrickMap;

DECLARE_MULTICAST_DELEGATE_OneParam(FWorldSetCurrMapDelegate, UMvBrickComponent*);

/**
 *
 */
UCLASS()
class MONUMENTVALLEY_API UMapHandlerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

public:
    void AddMap(AMvBrickMap* NewMap);

    void SetCurrMap(const FString& Target);

    AMvBrickMap* GetCurrMap() const { return CurrMap; }

protected:
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<AMvBrickMap>> Maps;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AMvBrickMap> CurrMap;

public:
    void AddSetCurrMapCallback(FWorldSetCurrMapDelegate::FDelegate&& Func);

protected:
    FWorldSetCurrMapDelegate OnSetCurrMap;
};
