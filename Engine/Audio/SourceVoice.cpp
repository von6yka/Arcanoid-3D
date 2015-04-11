#include "Engine/pch.h"

#include "Engine/Audio/SourceVoice.h"


namespace engine
{


SourceVoice::SourceVoice(IXAudio2* xAudio2, const WAVEFORMATEX& format)
{
    HRESULT hr = xAudio2->CreateSourceVoice(&_voice, &format);
    if (FAILED(hr)) {
        throw std::runtime_error("Can't create source voice");
    }
}


SourceVoice::~SourceVoice()
{
    stop();
    _voice->DestroyVoice();
}


void SourceVoice::play(const AudioClipPtr& audioClip)
{
    HRESULT hr;

    XAUDIO2_BUFFER buffer = {0};
    buffer.pAudioData = audioClip->data();
    buffer.AudioBytes = audioClip->dataSize();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    hr = _voice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) {
        throw std::runtime_error("Can't submit source voice");
    }

    hr = _voice->Start();
    if (FAILED(hr)) {
        throw std::runtime_error("Can't start source voice");
    }

    _audioClip = audioClip;
}


void SourceVoice::stop()
{
    _voice->Stop();
    _voice->FlushSourceBuffers();

    _audioClip.reset();
}


}//namespace engine
