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
    void OnSetMoveTarget(const FInputActionValue& Value);
    void OnSetControlBrick(const FInputActionValue& Value);

protected:
    struct FMouseInteractResult
    {
        FHitResult HitResult;
        FVector    MouseLocation;
        FVector    MouseDirection;
        FVector    CameraLocation;
        FRotator   CameraRotation;
    };

    TOptional<FMouseInteractResult> GetMouseInteractResult(const APlayerController* PlayerController) const;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monument Valley|Input")
    TObjectPtr<UInputMappingContext> MappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monument Valley|Input")
    TObjectPtr<UInputAction> SetMoveTargetAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monument Valley|Input")
    TObjectPtr<UInputAction> SetControlBrickAction;
};
