#include <iostream>
#include "CEngine.h"
#include "CWorld.h"
#include "CActor.h"
#include "ChimasUtilities.h"
#include "CEngineTexture.h"
#include "CSpriteComponent.h"
#include "Explosion.h"
#include "SpaceshipPawn.h"
#include "PlayerController.h"
#include "Rusher.h"
#include "Loner.h"
#include "Drone.h"
#include "StoneAsteroidBig.h"
#include "StoneAsteroidMedium.h"
#include "StoneAsteroidSmall.h"
#include "ChimasLog.h"
#include "CHUD.h"
#include "CTextWidget.h"
#include "GameManager.h"
#include "CImageWidget.h"

int main()
{
    CEngine gameEngine;

    float Width = 800;
    float height = 800;

    gameEngine.StartGame("Xenon 2000", Width, height, "Xenom/ImagesForGame/galaxy2.bmp");

    CWorld* world = gameEngine.GetWorld();

    if (world)
    {
        world->CreateBoundaryWalls(Width, height);

        // Spawn the Game Manager FIRST
        GameManager* gameManager = world->SpawnActor<GameManager>();

        // Spawn the player controller
        PlayerController* playerController = world->SpawnActor<PlayerController>();

        // Spawn spaceship pawn
        SpaceshipPawn* spaceshipPawn = world->SpawnActor<SpaceshipPawn>();
        if (spaceshipPawn)
        {
            spaceshipPawn->SetPosition(Vector2(Width / 2, (height / 5) * 4));
            spaceshipPawn->SetGameManager(gameManager); // Link to game manager

            // Have the player controller possess the spaceship pawn
            if (playerController)
            {
                playerController->Possess(spaceshipPawn);
                ChimasLog::Info("PlayerController possessed SpaceshipPawn");
            }
        }

        // NOTE: Enemies are now spawned by GameManager in waves
        // Remove manual enemy spawning from here

        world->BeginPlay();
    }
    else
    {
        std::cout << "World is null!" << std::endl;
    }

    gameEngine.Run();
    return 0;
}