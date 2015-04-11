#include "Engine/Engine.h"
#include "Engine/EngineController.h"

#include "Engine/Resource/ResourceStorage.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Scene/Scene.h"


using namespace engine;


class TestScene : public EngineController
{
public:
    void setEngine(Engine* core)
    {
        _engine = core;
        initScene();
    }

    bool exec(unsigned int deltaTime)
    {
        const float mouseSensivity = 0.06f;

        Camera* camera = _engine->scene()->camera();

        Vec3 mouseMove;
        if (_engine->inputSystem().isMouseMoved(&mouseMove)) {
            mouseMove *= mouseSensivity;

            if (_engine->inputSystem().isKeyDown(DIK_L)) {
                _light5->move(mouseMove);
            }
            else if (_engine->inputSystem().isKeyDown(DIK_M)) {
                static Vec3 or;
                or += mouseMove;
                _light5->setOrientation(Quat::fromAngleAxis(Vec3(1.0f, 0.0f, 0.0f), or.y) * Quat::fromAngleAxis(Vec3(0.0f, 1.0f, 0.0f), or.x));
            }
            else {                
                static Vec3 or;
                mouseMove.y = -mouseMove.y;
                or += mouseMove;
                camera->setOrientation(Quat::fromAngleAxis(Vec3(1.0f, 0.0f, 0.0f), or.y) * Quat::fromAngleAxis(Vec3(0.0f, 1.0f, 0.0f), or.x));
            }
        }

        if (_engine->inputSystem().isKeyDown(DIK_C)) {
            _light5->setOrientation(_light5->orientation() * Quat::fromAngleAxis(Vec3(0.0f, 1.0f, 0.0f), deltaTime * 0.01f));
        }


        if (_engine->inputSystem().isKeyClicked(DIK_Z)) {
            SpotLight* spotLight = static_cast<SpotLight*>(_light3->light());
            spotLight->setAngle(spotLight->angle() + 1);
        }

        if (_engine->inputSystem().isKeyClicked(DIK_X)) {
            SpotLight* spotLight = static_cast<SpotLight*>(_light3->light());
            spotLight->setAngle(spotLight->angle() - 1);
        }


        Vec3 positionDelta;

        if (_engine->inputSystem().isKeyDown(DIK_W)) {
            positionDelta += Vec3(0.0f, 0.0f, -1.0f);
        }

        if (_engine->inputSystem().isKeyDown(DIK_S)) {
            positionDelta += Vec3(0.0f, 0.0f, 1.0f);
        }

        if (_engine->inputSystem().isKeyDown(DIK_A)) {
            positionDelta += Vec3(-1.0f, 0.0f, 0.0f);
        }

        if (_engine->inputSystem().isKeyDown(DIK_D)) {
            positionDelta += Vec3(1.0f, 0.0f, 0.0f);
        }

        if (_engine->inputSystem().isKeyClicked(DIK_U)) {
            _light1->setPosition(Vec3(0.0f, 25.0f, 0.0f));
        }

        positionDelta *= deltaTime;

        if (positionDelta.length() > 0) {
            camera->move(camera->orientation().inversed() * positionDelta);
        }

        if (_engine->inputSystem().isKeyClicked(DIK_ESCAPE)) {
            return true;
        }

        return false;
    }

    void initScene()
    {
        Scene* scene = new Scene(_engine, L"TestScene");
        _engine->setScene(scene);

        Camera* camera = new Camera(NULL, L"Main camera");
        camera->setPosition(Vec3(-530.67267f, 414.44922f, 637.22992f));
        camera->setOrientation(Quat(0.12876517f, 0.33360270f, 0.046053898f, 0.93274212f));
        scene->addObject(camera);
        scene->setCamera(camera);

        Material groundMaterial;
        groundMaterial.shininess = 10.0f;
        groundMaterial.diffuseTexture = _engine->resourceStorage().getTexture(L"platform_diffuse.bmp");
        groundMaterial.normalTexture = _engine->resourceStorage().getTexture(L"platform_normal.bmp");
        groundMaterial.specularTexture = _engine->resourceStorage().getTexture(L"platform_specular.bmp");

        SceneObject* ground1 = new SceneObject(NULL, L"Ground");
        ground1->setMaterial(groundMaterial);
        ground1->setMesh(_engine->resourceStorage().getMesh(L"std:/mesh/cube/1000/2/1000"));
        scene->addObject(ground1);

        SceneObject* ground2 = new SceneObject(NULL, L"Ground");
        ground2->setPosition(Vec3(0.0f, 500.0f, 0.0f));
        ground2->setMaterial(groundMaterial);
        ground2->setMesh(_engine->resourceStorage().getMesh(L"std:/mesh/cube/1000/2/1000"));
        scene->addObject(ground2);

        Material boxMaterial;
        boxMaterial.diffuseTexture = _engine->resourceStorage().getTexture(L"wood_box_diffuse.bmp");
        boxMaterial.normalTexture = _engine->resourceStorage().getTexture(L"wood_box_normal.bmp");
        boxMaterial.specularTexture = _engine->resourceStorage().getTexture(L"wood_box_specular.bmp");

        SceneObject* box1 = new SceneObject(NULL, L"box1");
        box1->setPosition(Vec3(200.0f, 25.6f, 0.0f));
        box1->setMaterial(boxMaterial);
        box1->setMesh(_engine->resourceStorage().getMesh(L"std:/mesh/sphere/50/1000"));
        scene->addObject(box1);

        SceneObject* box2 = new SceneObject(NULL, L"box1");
        box2->setPosition(Vec3(-200.0f, 25.6f, 0.0f));
        box2->setMaterial(boxMaterial);
        box2->setMesh(_engine->resourceStorage().getMesh(L"std:/mesh/cube/50/50/50"));
        scene->addObject(box2);

        Material ballMaterial;
        ballMaterial.diffuseTexture = _engine->resourceStorage().getTexture(L"ball_diffuse.bmp");
        ballMaterial.normalTexture = _engine->resourceStorage().getTexture(L"ball_normal.bmp");
        ballMaterial.specularTexture = _engine->resourceStorage().getTexture(L"ball_specular.bmp");

        SceneObject* box3 = new SceneObject(NULL, L"box1");
        box3->setPosition(Vec3(0.0f, 100.0f, 200.0f));
        box3->setMaterial(ballMaterial);
        box3->setMesh(_engine->resourceStorage().getMesh(L"std:/mesh/sphere/50/500"));
        scene->addObject(box3);

        SceneObject* box4 = new SceneObject(NULL, L"box1");
        box4->setPosition(Vec3(0.0f, 50.1f, -100.0f));
        box4->setMaterial(boxMaterial);
        box4->setMesh(_engine->resourceStorage().getMesh(L"std:/mesh/cube/100/100/100"));
        scene->addObject(box4);

        SceneObject* box5 = new SceneObject(NULL, L"box1");
        box5->setPosition(Vec3(0.0f, 300.0f, 0.0f));
        box5->setMaterial(boxMaterial);
        box5->setMesh(_engine->resourceStorage().getMesh(L"std:/mesh/sphere/50/1000"));
        scene->addObject(box5);

        Material lightMaterial;
        lightMaterial.emission = Color(1.0f);
        lightMaterial.diffuseTexture = _engine->resourceStorage().getTexture(L"ball_diffuse.bmp");
        lightMaterial.normalTexture = _engine->resourceStorage().getTexture(L"ball_normal.bmp");
        lightMaterial.specularTexture = _engine->resourceStorage().getTexture(L"ball_specular.bmp");

        MeshPtr lightMesh = _engine->resourceStorage().getMesh(L"std:/mesh/sphere/10/100");

        //point
        if (1) {
            _light1 = new SceneObject(NULL, L"PointLight1");
            //light->setPosition(Vec3(-200.0f, 200.0f, -100.0f));
            _light1->setPosition(Vec3(0.0f, 25.0f, 0.0f));
            _light1->setMaterial(lightMaterial);
            _light1->setMesh(lightMesh);

            _light1->setLight(new PointLight(_light1));
            scene->addObject(_light1);
        }

        if (0) {
            _light2 = new SceneObject(NULL, L"PointLight2");
            _light2->setPosition(Vec3(200.0f, 100.0f, -100.0f));
            _light2->setMaterial(lightMaterial);
            _light2->setMesh(lightMesh);

            _light2->setLight(new PointLight(_light2));
            scene->addObject(_light2);
        }

        //spot
        if (1) {
            _light3 = new SceneObject(NULL, L"SpotLight1");
            _light3->setPosition(Vec3(0.0f, 100.0f, 0.0f));
            _light3->setMaterial(lightMaterial);
            _light3->setMesh(lightMesh);
            _light3->setOrientation(Quat::fromAngleAxis(Vec3(0.0f, 0.0f, 0.0f), 45.0f));

            _light3->setLight(new SpotLight(_light3));
            scene->addObject(_light3);
        }

        if (0) {
            _light4 = new SceneObject(NULL, L"SpotLight2");
            _light4->setPosition(Vec3(100.0f, 50.0f, -200.0f));
            _light4->setMaterial(lightMaterial);
            _light4->setMesh(lightMesh);

            _light4->setLight(new SpotLight(_light4));
            scene->addObject(_light4);
        }

        //direction
        if (1) {
            _light5 = new SceneObject(NULL, L"DirectLight");
            _light5->setPosition(Vec3(0, 500.0f, 0.0f));
            _light5->setMaterial(lightMaterial);
            _light5->setMesh(lightMesh);
            _light5->setOrientation(Quat::fromAngleAxis(Vec3(1.0f, 0.0f, 0.0f), -45.0f));

            _light5->setLight(new DirectionalLight(_light5));
            scene->addObject(_light5);
        }
    }

private:
    Engine* _engine;

    SceneObject* _light1;
    SceneObject* _light2;
    SceneObject* _light3;
    SceneObject* _light4;
    SceneObject* _light5;
};


int main(int, char*)
{
    Engine engine(L"Test scene");

    TestScene testScene;
    engine.exec(testScene);

	return 0;
}

