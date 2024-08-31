// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCore/Character/MvAICharacter.h"

// Sets default values
AMvAICharacter::AMvAICharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMvAICharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AMvAICharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMvAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
