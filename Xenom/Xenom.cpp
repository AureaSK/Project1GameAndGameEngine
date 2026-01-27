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
#include "ParallaxBackground.h"

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

        ParallaxBackground* parallaxBG = world->SpawnActor<ParallaxBackground>();
        if (parallaxBG)
        {
            parallaxBG->LoadSpriteSheet("Xenom/ImagesForGame/Blocks.bmp");
            parallaxBG->SetBaseScrollSpeed(30.0f);

            /*
             * SCREEN LAYOUT (800x800):
             *
             *   0                  400                800
             *   |-------------------|-------------------|
             *   |                   |                   |
             *   |   LEFT LAYER      |   RIGHT LAYER     |
             *   |   (400x800)       |   (400x800)       |
             *   |                   |                   |
             *   |-------------------|-------------------|
             *
             * Each layer is 400 pixels wide, positioned side-by-side
             * Position is the TOP-LEFT corner of each sprite
             */

             // LEFT LAYER - positioned at (0, 0) - top-left corner of screen
            parallaxBG->AddLayerFromSection(
                0, 832,      // Grab from sprite sheet at pixel (0, 832)
                400, 800,    // Size: 400 wide x 800 tall - NO STRETCHING!
                0.5f,        // Speed multiplier
                0.0f, 0.0f   // Screen position: LEFT edge (x=0, y=0)
            );

            parallaxBG->AddLayerFromSection(
                0, 832,      // Grab from sprite sheet at pixel (0, 832)
                400, 800,    // Size: 400 wide x 800 tall - NO STRETCHING!
                0.5f,        // Speed multiplier
                0.0f, 0.0f   // Screen position: LEFT edge (x=0, y=0)
            );

            ChimasLog::Info("✓ Parallax: 2 layers (400x800 each), side-by-side, NO STRETCH");
        }

        // Spawn the Game Manager
        GameManager* gameManager = world->SpawnActor<GameManager>();

        // Spawn the player controller
        PlayerController* playerController = world->SpawnActor<PlayerController>();

        // Spawn spaceship pawn
        SpaceshipPawn* spaceshipPawn = world->SpawnActor<SpaceshipPawn>();
        if (spaceshipPawn)
        {
            spaceshipPawn->SetPosition(Vector2(Width / 2, (height / 5) * 4));
            spaceshipPawn->SetGameManager(gameManager);

            if (playerController)
            {
                playerController->Possess(spaceshipPawn);
                ChimasLog::Info("PlayerController possessed SpaceshipPawn");
            }
        }

        world->BeginPlay();
    }
    else
    {
        std::cout << "World is null!" << std::endl;
    }

    gameEngine.Run();
    return 0;
}