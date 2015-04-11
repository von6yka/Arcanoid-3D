#pragma once

#include <list>
#include <map>

#include "Engine/Forwards.h"
#include "Engine/UniquePtr.h"

#include "Engine/Math/Vec2.h"
#include "Engine/Math/Vec3.h"
#include "Engine/Math/Plane.h"

#include "Engine/EngineController.h"


class Game : public engine::EngineController
{
public:
    Game();
    ~Game();

    void setEngine(engine::Engine* engine) override;

    bool exec(unsigned int deltaTime) override;

private:
    static const int LEVEL_APPEARANCE_TIME = 2 * 1000;

    engine::Engine* _engine;
    bool _shouldQuitGame;

    engine::WidgetLayer* _menuWidgetLayer;
    engine::UniquePtr<engine::Menu> _menu;

    engine::Scene* _scene;
    bool _isLevelInitialized;

    bool _isSceneAppeared;
    int _appearanceTimePassed;
    std::list<engine::SceneObject*> _appearanceList;

    engine::WidgetLayer* _gameWidgetLayer;
    engine::Label* _scoreLabel;
    engine::Label* _gameOverLabel;

    engine::SceneObject* _level;
    engine::SceneObject* _platform;
    engine::SceneObject* _ball;
    engine::Vec3 _ballVelocity;

    engine::Vec2 _cameraAngel;
    float _cameraDistance;

    int _score;
    bool _isGameOver;

    unsigned int _remainTime;

    void updateMenuItems();
    void setScore(int score);

    void onResumeGame();
    void onNewGame();
    void onQuit();

    void handleNotGameInput();

    void initializeScene();
    void initializeLevel();
    void resetLevel();
    void addSceneObject(engine::SceneObject* object);

    void handleSceneAppearance(unsigned int deltaTime);
    void updateGame(unsigned int deltaTime);
    void moveCamera(const engine::Vec3& mouseMove);
    void movePlatform(const engine::Vec3& mouseMove);
    void calcTick(unsigned int deltaTime);

    bool findMinimalIntersection(const engine::Vec3& l1, const engine::Vec3& l2, engine::SceneObject* ball,
        float* minimalLength, std::map< engine::SceneObject*, std::list<engine::Plane> >* intersectionMap);
};
