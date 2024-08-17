// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "MvPlayerAvatar.generated.h"

struct FInputActionValue;

class UInputAction;
class UInputMappingContext;

UCLASS()
class MONUMENTVALLEY_API AMvPlayerAvatar : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    AMvPlayerAvatar();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    void OnMouseClick(const FInputActionValue& Value);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monument Valley|Input")
    TObjectPtr<UInputMappingContext> MappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monument Valley|Input")
    TObjectPtr<UInputAction> MouseClickAction;
};
