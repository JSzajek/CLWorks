#pragma once

#include "Engine/EngineBaseTypes.h"
#include "Engine/World.h"
#include "EngineUtils.h"

class FTestUWorld
{
public:
    explicit FTestUWorld(const FURL& URL = FURL())
    {
        if (GEngine != nullptr)
        {
            static uint32 WorldCounter = 0;
            const FString WorldName = FString::Printf(TEXT("TestUWorld_%d"), WorldCounter++);

            if (UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, *WorldName, GetTransientPackage()))
            {
                FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
                WorldContext.SetCurrentWorld(World);

                World->InitializeActorsForPlay(URL);
                if (IsValid(World->GetWorldSettings()))
                {
                    // Need to do this manually since world doesn't have a game mode
                    World->GetWorldSettings()->NotifyBeginPlay();
                    World->GetWorldSettings()->NotifyMatchStarted();
                }
                World->BeginPlay();

                WeakWorld = MakeWeakObjectPtr(World);
            }
        }
    }

    UWorld* GetWorld() const { return WeakWorld.Get(); }

    ~FTestUWorld()
    {
        UWorld* World = WeakWorld.Get();
        if (World != nullptr && GEngine != nullptr)
        {
            World->EndPlay(EEndPlayReason::Type::RemovedFromWorld);

            GEngine->DestroyWorldContext(World);
            World->DestroyWorld(false);
        }
    }
private:
    TWeakObjectPtr<UWorld> WeakWorld;
};