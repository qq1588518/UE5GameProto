// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnWithCamera.generated.h"

UCLASS()
class UE5GAMEPROTO_API APawnWithCamera : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    APawnWithCamera();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(EditAnyWhere)
    class USpringArmComponent* springArmComp;

    UPROPERTY(EditAnyWhere)
    class UCameraComponent* cameraComp;

    UPROPERTY(EditAnyWhere)
    UStaticMeshComponent* staticMeshComp;

    // input variables
    FVector2D movementInput;
    FVector2D cameraInput;
    float zoomFactor;
    bool bZoomingIn;

    // input functions
    void moveForward(float axisValue);
    void moveRight(float axisValue);
    void pitchCamera(float axisValue);
    void yawCamera(float axisValue);
    void zoomIn();
    void zoomOut();

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
