// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "MvCharacterControlComponent.generated.h"

class AMvCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MONUMENTVALLEY_API UMvCharacterControlComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UMvCharacterControlComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    void SetControllingCharacter(AMvCharacter* Character) noexcept { ControllingCharacter = Character; }
    bool IsControlling() const noexcept { return ControllingCharacter != nullptr; }

    void TrySetCharacterTargetWorldLocation(FVector Target);

protected:
    TObjectPtr<AMvCharacter> ControllingCharacter = nullptr;
};
