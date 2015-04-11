#pragma once

#include <string>

#include "Engine/Forwards.h"

#include "Engine/Object.h"
#include "Engine/Resource/AudioClip.h"


namespace engine
{


class AudioSource : public Object
{
public:
    enum State {
        STATE_STOPPED,
        STATE_WAIT_PLAY,
        STATE_PLAYING,
        STATE_WAIT_STOP
    };

public:
    void* audioSystemData;

public:
    AudioSource(Object* parent, const std::wstring& name);

    void play();
    void stop();

public:
    AudioClipPtr audioClip() const
    {
        return _audioClip;
    }

    void setAudioClip(const AudioClipPtr& audioClip)
    {
        _audioClip = audioClip;
    }

    State state() const
    {
        return _state;
    }

    void setState(State state)
    {
        _state = state;
    }

private:
    AudioClipPtr _audioClip;

    State _state;
};


}//namespace engine
