#pragma once

#include "Engine/SharedPtr.h"


namespace engine
{

class Color;
class EngineController;
class Window;
class Scene;
class SceneObject;

class Mat4;

class GLRenderSystem;
class GLForwardPathSceneRenderer;
class GLGuiRenderer;
class GLTexture;

class InputReader;
class Camera;

class Material;
class ResourceStorage;

class Resource;
class AudioClip;
class Texture;
class Mesh;

typedef SharedPtr<Resource> ResourcePtr;
typedef SharedPtr<AudioClip> AudioClipPtr;
typedef SharedPtr<Texture> TexturePtr;
typedef SharedPtr<Mesh> MeshPtr;

class InputSystem;

class AudioSource;
class AudioSystem;

class WidgetLayer;
class Widget;
class Menu;
class Label;


}//namespace engine
