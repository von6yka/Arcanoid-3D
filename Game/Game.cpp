#include "Game/pch.h"

#include <sstream>

#include "Game/Game.h"

#include "Engine/Engine.h"
#include "Engine/Window.h"

#include "Engine/Math/Math.h"

#include "Engine/Input/InputSystem.h"

#include "Engine/Resource/ResourceStorage.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Resource/Mesh.h"

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Camera.h"

#include "Engine/Gui/WidgetLayer.h"
#include "Engine/Gui/Label.h"
#include "Engine/Gui/Menu.h"

#include "Engine/Audio/AudioSource.h"

const float levelPadding = 30.0f;
const float levelWidth = 400.0f;
const float levelHeight = 600.0f;
const float levelDepth = 400.0f;

const float platformWidth = 200.0f;
const float platformHeight = 10.0f;

const float SEC_PER_MILLISEC = 0.001f;

const float DEFAULT_CAMERA_DISTANCE = 1000.0f;
const float MAX_CAMERA_DISTANCE = 2000.0f;


using namespace engine;


Game::Game() : 
    _engine(NULL),
    _shouldQuitGame(false),
    _menuWidgetLayer(NULL),
    _scene(NULL),
    _isLevelInitialized(false),
    _isSceneAppeared(false),
    _appearanceTimePassed(0),
    _gameWidgetLayer(NULL),
    _scoreLabel(NULL),
    _gameOverLabel(NULL),
    _level(NULL),
    _platform(NULL),
    _ball(NULL),
    _cameraDistance(DEFAULT_CAMERA_DISTANCE),
    _score(0),
    _isGameOver(true),
    _remainTime(0)
{
}


Game::~Game()
{

}


void Game::setEngine(engine::Engine* engine)
{
    _engine = engine;

    _menuWidgetLayer = new WidgetLayer(_engine, L"Menu widget layer");
    _engine->addWidgetLayer(_menuWidgetLayer);

    _gameWidgetLayer = new WidgetLayer(_engine, L"Game widget layer");
    _engine->addWidgetLayer(_gameWidgetLayer);

    _scoreLabel = new Label(NULL, L"Score label");
    _scoreLabel->setText(L"Score: 0");
    _scoreLabel->setAnchor(Widget::ANCHOR_TOP_RIGHT);
    _gameWidgetLayer->addWidget(_scoreLabel);

    _gameOverLabel = new Label(NULL, L"Game over label");
    _gameOverLabel->setVisible(false);
    _gameOverLabel->setText(L"Game Over");
    _gameOverLabel->setAnchor(Widget::ANCHOR_CENTER);
    _gameWidgetLayer->addWidget(_gameOverLabel);

    updateMenuItems();
}


bool Game::exec(unsigned int deltaTime)
{
    handleNotGameInput();

    _gameWidgetLayer->setVisible(!_menuWidgetLayer->isVisible());

    if (!_menuWidgetLayer->isVisible()) {
        if (!_isLevelInitialized) {
            initializeLevel();
        }
        else if (!_isSceneAppeared) {
            handleSceneAppearance(deltaTime);
        }
        else {
            updateGame(deltaTime);
        }
    }

    return _shouldQuitGame;
}


void Game::updateMenuItems()
{
    _menu.reset(new Menu(NULL, L"Main menu"));
    _menu->setBackgroundColor(Color(0.2f, 0.5f));

    _menu->setOnChangeAudioClip(_engine->resourceStorage().getAudioClip(L"sound_1.wav"));
    _menu->setOnClickAudioClip(_engine->resourceStorage().getAudioClip(L"sound_1.wav"));

    if (!_isGameOver) {
        _menu->addItem(L"Resume Game", BindArgs0<Game>(this, &Game::onResumeGame));
    }
    _menu->addItem(L"New Game", BindArgs0<Game>(this, &Game::onNewGame));
    _menu->addItem(L"Quit", BindArgs0<Game>(this, &Game::onQuit));

    _menuWidgetLayer->addWidget(_menu.get());
}


void Game::setScore(int score)
{
    _score = score;

    std::wstringstream ss;
    ss << "Score: " << _score;

    _scoreLabel->setText(ss.str());
}


void Game::onResumeGame()
{
    _menuWidgetLayer->setVisible(false);
}


void Game::onNewGame()
{
    resetLevel();
    _menuWidgetLayer->setVisible(false);
}


void Game::onQuit()
{
    _shouldQuitGame = true;
}


void Game::handleNotGameInput()
{
    if (!_isGameOver || !_menuWidgetLayer->isVisible()) {
        if (_engine->inputSystem().isKeyClicked(DIK_ESCAPE)) {
            _menuWidgetLayer->setVisible(!_menuWidgetLayer->isVisible());
        }
    }
}


void Game::initializeLevel()
{
    _scene = new Scene(_engine, L"Main scene");
    _engine->setScene(_scene);

    Camera* camera = new Camera(NULL, L"Camera 1");
    _scene->setCamera(camera);
    camera->setPosition(Vec3(0.0f, 0.0f, _cameraDistance));
    _scene->addObject(camera);
    _cameraAngel = Vec2();
    _cameraDistance = DEFAULT_CAMERA_DISTANCE;
    moveCamera(Vec3());

    _ballVelocity = Vec3(0.0f, 500.0f, 0.0f);
    setScore(0);

    _isGameOver = false;
    _gameOverLabel->setVisible(_isGameOver);

    updateMenuItems();

    _isSceneAppeared = false;
    _appearanceList.clear();
    _appearanceTimePassed = 0;

    initializeScene();
    std::set<SceneObject*>::iterator i;
    for (i = _scene->objectSet().begin(); i != _scene->objectSet().end(); ++i) {
        SceneObject* sceneObject = *i;
        sceneObject->setVisible(false);
    }

    _isLevelInitialized = true;
}


void Game::resetLevel()
{
    delete _engine->scene();
    _engine->setScene(NULL);

    _isLevelInitialized = false;
}


void Game::initializeScene()
{
    //ball
    Material ballMaterial;
    ballMaterial.emission = Color(1.0f);
    ballMaterial.diffuseTexture = _engine->resourceStorage().getTexture(L"ball_diffuse.bmp");
    ballMaterial.normalTexture = _engine->resourceStorage().getTexture(L"ball_normal.bmp");
    ballMaterial.specularTexture = _engine->resourceStorage().getTexture(L"ball_specular.bmp");

    _ball = new SceneObject(NULL, L"Ball");
    _ball->setPosition(Vec3(0.0f, -levelHeight / 2.0f + 60.0f, 0.0f));
    _ball->setMaterial(ballMaterial);
    _ball->setMesh(_engine->resourceStorage().getMesh(L"std:/mesh/sphere/10/300"));
    _ball->setLight(new PointLight(_ball));
    _ball->setAudioSource(new AudioSource(_ball, L"Ball sound source"));
    _ball->audioSource()->setAudioClip(_engine->resourceStorage().getAudioClip(L"sound_2.wav"));
    addSceneObject(_ball);

    //level
    Material levelMaterial;
    levelMaterial.diffuseTexture = _engine->resourceStorage().getTexture(L"brick_wall_diffuse.bmp");
    levelMaterial.normalTexture = _engine->resourceStorage().getTexture(L"brick_wall_normal.bmp");
    levelMaterial.specularTexture = _engine->resourceStorage().getTexture(L"brick_wall_specular.bmp");

    _level = new SceneObject(NULL, L"Level");
    _level->setMaterial(levelMaterial);
    {
        std::wstringstream ss;
        ss << "std:/mesh/cube/" << levelWidth << "/" << levelHeight << "/" << levelDepth << "/inverted";
        _level->setMesh(_engine->resourceStorage().getMesh(ss.str()));
    }
    addSceneObject(_level);

    //platform
    Material platformMaterial;
    platformMaterial.diffuseTexture = _engine->resourceStorage().getTexture(L"platform_diffuse.bmp");
    platformMaterial.normalTexture = _engine->resourceStorage().getTexture(L"platform_normal.bmp");
    platformMaterial.specularTexture = _engine->resourceStorage().getTexture(L"platform_specular.bmp");

    MeshPtr platformMesh;
    {
        std::wstringstream ss;
        ss << "std:/mesh/cube/" << platformWidth << "/" << platformHeight << "/" << platformWidth;
        platformMesh = _engine->resourceStorage().getMesh(ss.str());
    }

    _platform = new SceneObject(NULL, L"Platform");
    _platform->setPosition(Vec3(0.0f, -levelHeight / 2.0f + 50.0f, 0.0f));
    _platform->setMaterial(platformMaterial);
    _platform->setMesh(platformMesh);
    addSceneObject(_platform);

    engine::SceneObject* platformLight = new SceneObject(_platform, L"Platform light");
    platformLight->setOrientation(Quat::fromAngleAxis(Vec3(1.0f, 0.0f, 0.0f), 90.0f));
    SpotLight* spotLight = new SpotLight(platformLight);
    spotLight->setAngle(45.0f);
    spotLight->setAttenuation(Vec3(0.01f, 0.0005f, 0.00001f));
    platformLight->setLight(spotLight);
    addSceneObject(platformLight);

    //blocks
    const float xNumber = 5;
    const float yNumber = 3;
    const float zNumber = 5;
    const float blockPadding = 2.0f;

    const float blockWidth = (levelWidth - levelPadding * 2 - blockPadding * (xNumber - 1)) / xNumber;
    const float blockHeight = 50.0f;
    const float blockDepth = (levelDepth - levelPadding * 2 - blockPadding * (xNumber - 1)) / zNumber;

    Material blockMaterial;
    blockMaterial.diffuseTexture = _engine->resourceStorage().getTexture(L"wood_box_diffuse.bmp");
    blockMaterial.normalTexture = _engine->resourceStorage().getTexture(L"wood_box_normal.bmp");
    blockMaterial.specularTexture = _engine->resourceStorage().getTexture(L"wood_box_specular.bmp");

    MeshPtr blockMesh;
    {
        std::wstringstream ss;
        ss << "std:/mesh/cube/" << blockWidth << "/" << blockHeight << "/" << blockDepth;
        blockMesh = _engine->resourceStorage().getMesh(ss.str());
    }

    for (int x = 0; x < xNumber; ++x) {
        for (int y = 0; y < yNumber; ++y) {
            for (int z = 0; z < zNumber; ++z) {
                float pX = -levelWidth / 2.0f + blockWidth  / 2.0f + (blockWidth  + blockPadding) * x + levelPadding;
                float pY = levelHeight / 2.0f - blockHeight / 2.0f - (blockHeight + blockPadding) * y - levelPadding;
                float pZ = -levelDepth / 2.0f + blockDepth  / 2.0f + (blockDepth  + blockPadding) * z + levelPadding;

                std::wstringstream ss;
                ss << "Block: x - " << x << " y - " << y << " z - " << z;

                SceneObject* block = new SceneObject(NULL, ss.str());
                block->setPosition(Vec3(pX, pY, pZ));
                block->setMaterial(blockMaterial);
                block->setMesh(blockMesh);
                addSceneObject(block);
            }
        }
    }
}


void Game::addSceneObject(engine::SceneObject* object)
{
    _scene->addObject(object);
    _appearanceList.push_back(object);
}


void Game::handleSceneAppearance(unsigned int deltaTime)
{
    const size_t k = LEVEL_APPEARANCE_TIME / _appearanceList.size();
    const size_t numberObjectShouldVisible = _appearanceTimePassed / k;

    size_t visibleObjectCount = 0;

    std::list<engine::SceneObject*>::iterator i;
    for (i = _appearanceList.begin(); i != _appearanceList.end(); ++i) {
        if (visibleObjectCount == numberObjectShouldVisible) {
            break;
        }

        SceneObject* object = *i;
        object->setVisible(true);

        ++visibleObjectCount;
    }

    _isSceneAppeared = (visibleObjectCount == _appearanceList.size());

    _appearanceTimePassed += deltaTime;
}


void Game::updateGame(unsigned int deltaTime)
{
    const float cameraMoveSens = 0.1f;
    const float gameMoveSens = 0.3f;

    Vec3 mouseMove;

    if (_engine->inputSystem().isMouseMoved(&mouseMove)) {
        if (_engine->inputSystem().isMouseDown(InputSystem::MOUSE_BUTTON2)) {
            moveCamera(mouseMove * cameraMoveSens);
        }
        else {
            if (mouseMove.z != 0) {
                moveCamera(Vec3(0, 0, mouseMove.z * cameraMoveSens));
            }

            movePlatform(mouseMove * gameMoveSens);
        }
    }

    if (!_isGameOver) {
        const int tickDuration = 10;
        int totalTime = _remainTime + deltaTime;
        int tickCount = totalTime / tickDuration;
        for (int i = 0; i < tickCount; ++i) {
            calcTick(tickDuration);
        }

        _remainTime = totalTime % tickDuration;
    }
}


void Game::moveCamera(const Vec3& mouseMove)
{
    _cameraAngel.x += mouseMove.x;
    _cameraAngel.y -= mouseMove.y;

    _cameraAngel.x = Math::ufmod(_cameraAngel.x, Math::TWO_PI_DEGREE);
    _cameraAngel.y = Math::clamp(_cameraAngel.y, -90.0f, 90.0f);

    Quat yAxisRotation = Quat::fromAngleAxis(Vec3(1.0f, 0.0f, 0.0f), _cameraAngel.y);
    Quat xAxisRotation = Quat::fromAngleAxis(Vec3(0.0f, 1.0f, 0.0f), _cameraAngel.x);

    Quat rotation = yAxisRotation * xAxisRotation;

    _engine->scene()->camera()->setOrientation(rotation);

    _cameraDistance += mouseMove.z;
    _cameraDistance = Math::clamp(_cameraDistance, 0.0f, MAX_CAMERA_DISTANCE);

    const Quat& orientation = _engine->scene()->camera()->orientation();
    _engine->scene()->camera()->setPosition(orientation.inversed() * Vec3(0.0f, 0.0f, _cameraDistance));
}


void Game::movePlatform(const Vec3& mouseMove)
{
    Vec3 move(mouseMove.x, 0.0f, mouseMove.y);

    if (_cameraAngel.y < 0) {
        move.z *= -1;
    }

    float angel = Math::ufmod(_cameraAngel.x, Math::TWO_PI_DEGREE);

    if (angel < 45.0f || angel > 90.0f + 90.0f + 90.0f + 45.0f) {
        move.z *= -1;
    }
    else if (angel > 90.0f + 90.0f + 45.0f) {
        move.z *= -1;
        move.x *= -1;
        std::swap(move.x, move.z);
    }
    else if (angel > 90.0f + 45.0f) {
        move.x *= -1;
    }
    else {
        std::swap(move.x, move.z);
    }

    _platform->move(move);

    float x = Math::clamp(_platform->position().x, (-levelWidth + platformWidth) / 2, (levelWidth - platformWidth) / 2);
    float z = Math::clamp(_platform->position().z, (-levelDepth + platformWidth) / 2, (levelDepth - platformWidth) / 2);
    _platform->setPosition(Vec3(x, _platform->position().y, z));
}


void Game::calcTick(unsigned int tickDeltaTime)
{
    Vec3 ballMovement = _ballVelocity * (tickDeltaTime * SEC_PER_MILLISEC);

    Vec3 ballPrevPosition = _ball->position();
    Vec3 ballNewPosition = _ball->position() + ballMovement;

    bool hasInsolubleSituation = false; //FIXME: remove

    float minimalLength;
    std::map< SceneObject*, std::list<Plane> > intersectionMap;
    while (findMinimalIntersection(ballPrevPosition, ballNewPosition, _ball, &minimalLength, &intersectionMap))
    {
        if (!Math::isFinite(minimalLength)) {
            hasInsolubleSituation = true;
            break;
        }

        float movementPart = minimalLength / ballMovement.length();
        ballPrevPosition = ballPrevPosition + ballMovement * movementPart;

        Vec3 reflectionVector;

        std::map< SceneObject*, std::list<Plane> >::iterator mapIt;
        for (mapIt = intersectionMap.begin(); mapIt != intersectionMap.end(); ++mapIt) {
            SceneObject* sceneObject = mapIt->first;

            std::list<Plane>::iterator i;
            for (i = mapIt->second.begin(); i != mapIt->second.end(); ++i) {
                reflectionVector += i->normal();

                if (sceneObject == _level) {
                    if (i->normal().isFuzzyEqual(Vec3(0.0f, 1.0f, 0.0f))) {
                        _isGameOver = true;
                        _gameOverLabel->setVisible(_isGameOver);
                        updateMenuItems();
                    }
                }
            }

            if (sceneObject != _level && sceneObject != _platform && sceneObject != _ball) {
                sceneObject->setVisible(false);
                setScore(_score + 1);
            }
        }

        reflectionVector.normalize();
        if (!reflectionVector.isFinite()) {
            hasInsolubleSituation = true;
            break;
        }

        ballMovement *= 1.0f - movementPart;

        ballMovement.reflect(reflectionVector);
        _ballVelocity.reflect(reflectionVector);

        if (intersectionMap.size() == 1) {
            SceneObject* object = intersectionMap.begin()->first;

            if (object == _platform) {
                Vec3 deviation = ballNewPosition - _platform->position();
                deviation.y = 0;
                float part = deviation.length() / platformWidth;
                deviation = deviation.normalized() * part;

                if (deviation.isFinite()) {
                    Vec3 newBallDirection = Vec3(0.0f, 1.0f, 0.0f) + deviation;

                    newBallDirection.normalize();
                    ballMovement = newBallDirection * ballMovement.length();                
                    _ballVelocity = newBallDirection * _ballVelocity.length();
                }
            }
        }

        ballNewPosition = ballPrevPosition + ballMovement;

        _ball->audioSource()->play();
    }

    if (!hasInsolubleSituation) {
        _ball->setPosition(ballNewPosition);
    }
}


bool Game::findMinimalIntersection(const Vec3& l1, const Vec3& l2, SceneObject* ball,
    float* minimalLength, std::map< engine::SceneObject*, std::list<engine::Plane> >* intersectionMap)
{
    bool hasIntersection = false;

    Scene::SceneObjectSet::const_iterator i;
    for (i = _engine->scene()->objectSet().begin(); i != _engine->scene()->objectSet().end(); ++i) {
        SceneObject* sceneObject = *i;
        if (sceneObject == ball || !sceneObject->isVisible() || sceneObject->mesh().isNull()) {
            continue;
        }

        Mat4 modelMatrix = Mat4::translation(sceneObject->position()) * sceneObject->orientation().toMat4();

        size_t triangularCount = sceneObject->mesh()->triangularCount();

        Vec3 p1;
        Vec3 p2;
        Vec3 p3;
        for (size_t triangularNumber = 0; triangularNumber < triangularCount; ++triangularNumber) {
            sceneObject->mesh()->triangularPosition(triangularNumber, &p1, &p2, &p3);

            p1 = modelMatrix * p1;
            p2 = modelMatrix * p2;
            p3 = modelMatrix * p3;

            Vec3 intersectionPoint;
            if (Math::isIntersect(p1, p2, p3, l1, l2, &intersectionPoint)) {
                if (!l1.isFuzzyEqual(intersectionPoint)) {
                    float length = (intersectionPoint - l1).length();

                    if (!hasIntersection || Math::fuzzyCompare(*minimalLength, length) > 0) {
                        *minimalLength = length;
                        hasIntersection = true;

                        intersectionMap->clear();
                    }

                    if (Math::isFuzzyEqual(*minimalLength, length)) {
                        (*intersectionMap)[sceneObject].push_back(Plane(p1, p2, p3));
                    }
                }
            }
        }
    }

    return hasIntersection;
}
