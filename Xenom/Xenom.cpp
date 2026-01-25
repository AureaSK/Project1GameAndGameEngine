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
#include "ChimasLog.h"
#include "CHUD.h"
#include "CTextWidget.h"
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

        CHUD* hud = world->GetHUD();

        if (hud)
        {
            CTextWidget* centerText = hud->CreateWidget<CTextWidget>();
            std::string fontPath = gameEngine.ResolveAssetPath("Xenom/ImagesForGame/font16x16.bmp");
            centerText->LoadGridFont(fontPath, 16,16," !~#$%&'()*+,-./0123456789:;<=>?ÇABCDEFGHIJKLMNOPQRSTUVWXYZ[¨]^»`abcdefghijklmnopqrstuvwxyz{|}ªº");


            centerText->SetText("I Love Hot dog");
            /*
            centerText->SetPosition(Vector2(0.0f, height / 2.0f - 20.0f));
            centerText->SetSize(Vector2(Width, 40.0f));
            centerText->SetHorizontalAlignment(CTextWidget::TextAlign::Center);

            centerText->SetScale(3.0f);
            //centerText->SetColor(Color::Magenta());
            */
            centerText->SetPosition(Vector2(0.0f, 0.0f));
            centerText->SetSize(Vector2(Width, 5.f));
            centerText->SetHorizontalAlignment(CTextWidget::TextAlign::Center);

            ChimasLog::Info("UI Text created: 'I Love Hot dog'");
        }

        // Spawn the player controller
        PlayerController* playerController = world->SpawnActor<PlayerController>();

        // Spawn spaceship pawn
        SpaceshipPawn* spaceshipPawn = world->SpawnActor<SpaceshipPawn>();
        if (spaceshipPawn)
        {
            spaceshipPawn->SetPosition(Vector2(Width / 2, (height / 5) * 4));

            // Have the player controller possess the spaceship pawn
            if (playerController)
            {
                playerController->Possess(spaceshipPawn);
                ChimasLog::Info("PlayerController possessed SpaceshipPawn");
            }
        }

        // Spawn a rusher
        Rusher* rusher = world->SpawnActor<Rusher>();
        if (rusher)
        {
            rusher->SetPosition(Vector2(400.f, 100.f));
        }

        // Spawn loners
        for (int i = 0; i < 3; i++)
        {
            Loner* loner = world->SpawnActor<Loner>();
            if (loner)
            {
                loner->SetPosition(Vector2(100.0f + i * 200.0f, 100.0f + i * 50.0f));
                loner->SetDirection(1.0f);
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