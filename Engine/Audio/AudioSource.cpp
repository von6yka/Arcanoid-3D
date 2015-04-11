#include "Engine/pch.h"

#include "Engine/Audio/AudioSource.h"
#include "Engine/Audio/AudioSystem.h"


namespace engine
{


AudioSource::AudioSource(Object* parent, const std::wstring& name) :
    Object(parent, name),
    audioSystemData(NULL),
    _state(STATE_STOPPED)
{

}


void AudioSource::play()
{
    _state = STATE_WAIT_PLAY;
}


void AudioSource::stop()
{
    _state = STATE_WAIT_STOP;
}


}//namespace engine
