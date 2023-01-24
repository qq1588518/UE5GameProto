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
    springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

    // set relative location and rotation when adding mesh
    GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FQuat(FRotator(0.f, -90.f, 0.f)));

    // attach components
    springArmComp->SetupAttachment(GetMesh());
    cameraComp->SetupAttachment(springArmComp, USpringArmComponent::SocketName);
    
    // assign SpringArm variables
    springArmComp->bUsePawnControlRotation = true;
    springArmComp->TargetArmLength = 450.0f;
    springArmComp->bEnableCameraLag = true; // camera smooth
    springArmComp->CameraLagSpeed = 3.0f;

    // assign character movement component variables
    auto charaMoveComp = GetCharacterMovement();
    charaMoveComp->bOrientRotationToMovement = true;
    charaMoveComp->bUseControllerDesiredRotation = true;
    charaMoveComp->bIgnoreBaseRotation = true;
    charaMoveComp->NavAgentProps.bCanCrouch = true;

    //// take control of the default Player
    //AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AAPlayer::BeginPlay()
{
    Super::BeginPlay();

    // initialize camera position and rotation
    springArmComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 100.f), FRotator(-60.0f, 0.0f, 0.0f));
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

    // hook input movement
    InputComponent->BindAxis("MoveForward", this, &AAPlayer::moveForward);
    InputComponent->BindAxis("MoveRight", this, &AAPlayer::moveRight);

    // hook input camera rotation
    InputComponent->BindAxis("CameraPitch", this, &APawn::AddControllerPitchInput);
    InputComponent->BindAxis("CameraYaw", this, &APawn::AddControllerYawInput);

    // hook camera zoom
    InputComponent->BindAction("ZoomIn", IE_Pressed, this, &AAPlayer::zoomIn);
    InputComponent->BindAction("ZoomIn", IE_Released, this, &AAPlayer::zoomOut);

    // hook action
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AAPlayer::beginSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AAPlayer::endSprint);
    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AAPlayer::beginCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AAPlayer::endCrouch);
}

void AAPlayer::moveForward(float axisValue) {
    if (Controller != nullptr && axisValue != 0.f) {
        // Decide direction
        const auto rotation = Controller->GetControlRotation();
        const FRotator yawRotation(0, rotation.Yaw, 0);
        // Get the transformed x-axis orientation from the rotation matrix
        // Consider that it is right-handed coordinate systems
        const auto direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);

        // Add movement scaled by input axisValue
        AddMovementInput(direction, axisValue);
    }
}

void AAPlayer::moveRight(float axisValue) {
    if (Controller != nullptr && axisValue != 0.f) {
        // Decide right direction
        const auto rotation = Controller->GetControlRotation();
        const FRotator yawRotation(0, rotation.Yaw, 0);
        const auto direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

        // Add movement scaled by input axisValue
        AddMovementInput(direction, axisValue);
    }
}

void AAPlayer::beginSprint() {
    GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
}

void AAPlayer::endSprint() {
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AAPlayer::beginCrouch() {
    Crouch();
}

void AAPlayer::endCrouch() {
    UnCrouch();
}

void AAPlayer::zoomIn() {
    bZoomingIn = true;
}

void AAPlayer::zoomOut() {
    bZoomingIn = false;
}
