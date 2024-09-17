// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MvAICharacter.generated.h"

class UMvMovementComponent;

UCLASS()
class MONUMENTVALLEY_API AMvAICharacter : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AMvAICharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UMvMovementComponent> MovementComp;
};
