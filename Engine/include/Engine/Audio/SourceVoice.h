#pragma once

#include "Engine/NonCopyable.h"
#include "Engine/Resource/AudioClip.h"

#include <Xaudio2.h>

namespace engine
{


class SourceVoice : NonCopyable
{
public:
    SourceVoice(IXAudio2* xAudio2, const WAVEFORMATEX& format);
    ~SourceVoice();

public:
    void play(const AudioClipPtr& audioClip);
    void stop();

    IXAudio2SourceVoice* internal()
    {
        return _voice;
    }

private:
    IXAudio2SourceVoice* _voice;

    AudioClipPtr _audioClip;
};


}//namespace engine
