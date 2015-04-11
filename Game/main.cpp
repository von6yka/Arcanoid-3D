#include "game/pch.h"

#include <assert.h>

#include "windows.h"

#include "Engine/Engine.h"
#include "Game/Game.h"

using namespace engine;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    {
        Engine engine(L"Arcanoid 3D");

        Game game;
        engine.exec(game);
    }

    assert(Object::count == 0);

    return 0;
}
