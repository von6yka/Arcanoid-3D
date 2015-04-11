#pragma once

#include <Xaudio2.h>

#include "Engine/SharedPtr.h"
#include "Engine/Resource/Resource.h"


namespace engine
{


class AudioClip : public Resource
{
public:
    AudioClip(ResourceStorage& storage, const std::vector<BYTE>& data, const WAVEFORMATEX& format) :
        Resource(storage, TYPE_AUDIO_CLIP),
        _data(data),
        _format(format)
    {
    }

    ~AudioClip()
    {

    }

    const BYTE* data() const
    {
        return &_data[0];
    }

    size_t dataSize() const
    {
        return _data.size();
    }

    const WAVEFORMATEX& format() const
    {
        return _format;
    }

private:
    std::vector<BYTE> _data;
    WAVEFORMATEX _format;
};


}//namespace engine
