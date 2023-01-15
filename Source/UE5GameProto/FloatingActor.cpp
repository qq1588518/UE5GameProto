// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingActor.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AFloatingActor::AFloatingActor() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    visualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    visualMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> cubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));

    if (cubeVisualAsset.Succeeded()) {
        visualMesh->SetStaticMesh(cubeVisualAsset.Object);
        visualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    }
}

// Called when the game starts or when spawned
void AFloatingActor::BeginPlay() {
    Super::BeginPlay();
    UKismetSystemLibrary::PrintString(this, "hello world", true, true, FColor::Red, 2.f, TEXT("None"));
}

// Called every frame
void AFloatingActor::Tick(float deltaTime) {
    Super::Tick(deltaTime);

    FVector location = GetActorLocation();
    FRotator rotation = GetActorRotation();
    float runningTime = GetGameTimeSinceCreation();

    float deltaHeight = (FMath::Sin(runningTime + deltaTime) - FMath::Sin(runningTime));

    location.Z += deltaHeight * 150.0f;
    float deltaRotation = deltaTime * 150.0f;
    rotation.Yaw += deltaRotation;

    SetActorLocationAndRotation(location, rotation);
}
