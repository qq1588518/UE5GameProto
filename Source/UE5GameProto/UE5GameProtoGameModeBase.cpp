// Copyright Epic Games, Inc. All Rights Reserved.


#include "UE5GameProtoGameModeBase.h"

AUE5GameProtoGameModeBase::AUE5GameProtoGameModeBase() : Super() {
    FString path = "/Game/BP/BpAPlayer.BpAPlayer_c";
    ConstructorHelpers::FObjectFinder<UClass> characterBP(*path);
    TSubclassOf<AActor> bpAPlayer = characterBP.Object;

    if (bpAPlayer != nullptr) {
        DefaultPawnClass = bpAPlayer;
    }
}