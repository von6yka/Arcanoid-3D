#pragma once

#include <list>

#include "Engine/Forwards.h"

#include "Engine/Audio/SourceVoice.h"

#include "Engine/Resource/AudioClip.h"

#include <Xaudio2.h>
#include <x3daudio.h>

namespace engine
{


class AudioSystem : NonCopyable
{

public:
    AudioSystem();
    ~AudioSystem();

    void play(const AudioClipPtr& audioClip);

    void update(const Scene* scene);

private:
    IXAudio2* _xAudio2;
    IXAudio2MasteringVoice* _masteringVoice;
    X3DAUDIO_HANDLE _3dAudio;

    typedef SharedPtr<SourceVoice> SourceVoicePtr;
    typedef std::list<SourceVoicePtr> SourceVoiceList;
    SourceVoiceList _freeVoiceList;
    SourceVoiceList _sceneVoiceList;

    void update3DParams(const Camera& camera, const SceneObject& sceneObject, SourceVoice* voice);
};


}//namespace engine
