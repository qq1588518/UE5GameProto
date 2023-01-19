// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AAPlayer::AAPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // create components
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

    // attach components
    UCapsuleComponent* capsule = GetCapsuleComponent();
    capsule->SetupAttachment(RootComponent);
    springArmComp->SetupAttachment(capsule);
    cameraComp->SetupAttachment(springArmComp, USpringArmComponent::SocketName);

    // assign SpringArm variables
    springArmComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.f), FRotator(-60.0f, 0.0f, 0.0f));
    springArmComp->TargetArmLength = 450.0f;
    springArmComp->bEnableCameraLag = true; // camera smooth
    springArmComp->CameraLagSpeed = 3.0f;

    // take control of the default Player
    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AAPlayer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // set camera FOV and arm length by zooming in/out
    {
        if (bZoomingIn) {
            zoomFactor += DeltaTime / 0.5f;
        } else {
            zoomFactor -= DeltaTime / 0.25f;
        }
        zoomFactor = FMath::Clamp<float>(zoomFactor, 0.0f, 1.0f);

        cameraComp->FieldOfView = FMath::Lerp<float>(90.0f, 60.0f, zoomFactor);
        springArmComp->TargetArmLength = FMath::Lerp<float>(400.0f, 300.0f, zoomFactor);
    }

    // set actor yaw by cameraInput
    {
        FRotator actorRotation = GetActorRotation();
        actorRotation.Yaw += cameraInput.X;
        SetActorRotation(actorRotation);
    }

    // set camera arm pitch by cameraInput
    {
        FRotator armRotation = springArmComp->GetComponentRotation();
        armRotation.Pitch = FMath::Clamp(armRotation.Pitch + cameraInput.Y, -80.0f, -15.0f);
        springArmComp->SetWorldRotation(armRotation);
    }

    // actor movement by input
    {
        if (!movementInput.IsZero()) {
            FVector2D inputNormal = movementInput.GetSafeNormal();
            FVector actorLocation = GetActorLocation();
            actorLocation += GetActorForwardVector() * inputNormal.X * DeltaTime * 300;
            actorLocation += GetActorRightVector() * inputNormal.Y * DeltaTime * 300;
            SetActorLocation(actorLocation);
        }
    }
}

// Called to bind functionality to input
void AAPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    // hook zoomIn
    InputComponent->BindAction("ZoomIn", IE_Pressed, this, &AAPlayer::zoomIn);
    InputComponent->BindAction("ZoomIn", IE_Released, this, &AAPlayer::zoomOut);

    // hook input movement
    InputComponent->BindAxis("MoveForward", this, &AAPlayer::moveForward);
    InputComponent->BindAxis("MoveRight", this, &AAPlayer::moveRight);
    InputComponent->BindAxis("CameraPitch", this, &AAPlayer::pitchCamera);
    InputComponent->BindAxis("CameraYaw", this, &AAPlayer::yawCamera);
}

void AAPlayer::moveForward(float axisValue) {
    movementInput.X = FMath::Clamp<float>(axisValue, -1.0f, 1.0f);
}

void AAPlayer::moveRight(float axisValue) {
    movementInput.Y = FMath::Clamp<float>(axisValue, -1.0f, 1.0f);
}

void AAPlayer::pitchCamera(float axisValue) {
    cameraInput.Y = axisValue;
}

void AAPlayer::yawCamera(float axisValue) {
    cameraInput.X = axisValue;
}

void AAPlayer::zoomIn() {
    bZoomingIn = true;
}

void AAPlayer::zoomOut() {
    bZoomingIn = false;
}
