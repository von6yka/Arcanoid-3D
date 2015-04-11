#include "Engine/pch.h"

#include "Engine/Audio/AudioSystem.h"
#include "Engine/Audio/AudioSource.h"

#include "Engine/Scene/Scene.h"


namespace engine
{


static X3DAUDIO_VECTOR getVector(float x, float y, float z)
{
    X3DAUDIO_VECTOR vector = {x, y, z};
    return vector;
}


AudioSystem::AudioSystem()
{
    if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) {
        throw std::runtime_error("Can't initialize COM");
    }

    UINT32 flags = 0;
#ifdef _DEBUG
    flags = XAUDIO2_DEBUG_ENGINE;
#endif

    if (FAILED(XAudio2Create(&_xAudio2, flags))) {
        throw std::runtime_error("Can't create XAudio2");
    }

    if (FAILED(_xAudio2->CreateMasteringVoice(&_masteringVoice))) {
        throw std::runtime_error("Can't create master voice");
    }

    XAUDIO2_DEVICE_DETAILS soundDetails;
    if (FAILED(_xAudio2->GetDeviceDetails(0, &soundDetails))) {
        throw std::runtime_error("Can't get device details");
    }

    X3DAudioInitialize(soundDetails.OutputFormat.dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, _3dAudio);
}


AudioSystem::~AudioSystem()
{
    _freeVoiceList.clear();
    _sceneVoiceList.clear();

    _masteringVoice->DestroyVoice();
    _xAudio2->Release();

    CoUninitialize();
}


void AudioSystem::play(const AudioClipPtr& audioClip)
{
    SourceVoicePtr voice(new SourceVoice(_xAudio2, audioClip->format()));
    voice->play(audioClip);

    _freeVoiceList.push_back(voice);
}


void AudioSystem::update(const Scene* scene)
{
    SourceVoiceList expiredSceneVoiceList = _sceneVoiceList;

    if (scene != NULL && scene->camera() != NULL) {
        Scene::SceneObjectList sceneObjectList = scene->objectListWithAudioSource();

        Scene::SceneObjectList::iterator i;
        for (i = sceneObjectList.begin(); i != sceneObjectList.end(); ++i) {
            SceneObject* sceneObject = *i;

            AudioSource* audioSource = sceneObject->audioSource();
            assert(audioSource != NULL);

            SourceVoice* voice = static_cast<SourceVoice*>(audioSource->audioSystemData);

            SourceVoiceList::iterator i;
            for (i = expiredSceneVoiceList.begin(); i != expiredSceneVoiceList.end(); ++i) {
                SourceVoicePtr sourceVoice(*i);

                if (sourceVoice.get() == voice) {
                    expiredSceneVoiceList.erase(i);
                    break;
                }
            }

            if (audioSource->state() == AudioSource::STATE_WAIT_PLAY) {
                if (!audioSource->audioClip().isNull()) {
                    if (voice == NULL) {
                        voice = new SourceVoice(_xAudio2, audioSource->audioClip()->format());
                        audioSource->audioSystemData = voice;

                        _sceneVoiceList.push_back(SourceVoicePtr(voice));
                    }
                    else {
                        voice->stop();
                    }

                    update3DParams(*scene->camera(), *sceneObject, voice);
                    voice->play(audioSource->audioClip());

                    audioSource->setState(AudioSource::STATE_PLAYING);
                }
            }
            else if (audioSource->state() == AudioSource::STATE_PLAYING) {
                XAUDIO2_VOICE_STATE state;
                voice->internal()->GetState(&state);

                if (state.BuffersQueued != 0) {
                    update3DParams(*scene->camera(), *sceneObject, voice);
                }
                else {
                    audioSource->setState(AudioSource::STATE_STOPPED);
                    audioSource->audioSystemData = NULL;
                }
            }
            else if (audioSource->state() == AudioSource::STATE_WAIT_STOP) {
                audioSource->setState(AudioSource::STATE_STOPPED);
                audioSource->audioSystemData = NULL;
            }
        }
    }

    while (!expiredSceneVoiceList.empty()) {
        _sceneVoiceList.remove(expiredSceneVoiceList.front());
        expiredSceneVoiceList.pop_front();
    }

    SourceVoiceList::iterator i = _freeVoiceList.begin(); 
    while (i != _freeVoiceList.end()) {
        SourceVoicePtr voice(*i);

        XAUDIO2_VOICE_STATE state;
        voice->internal()->GetState(&state);

        if (state.BuffersQueued == 0) {
            i = _freeVoiceList.erase(i);
        }
        else {
            ++i;
        }
    }
}


void AudioSystem::update3DParams(const Camera& camera, const SceneObject& sceneObject, SourceVoice* voice)
{
    X3DAUDIO_CONE listenerCone = X3DAudioDefault_DirectionalCone;

    X3DAUDIO_LISTENER listener;
    listener.Velocity = getVector(0, 0, 0); //FIXME:
    listener.pCone = &listenerCone;
    listener.OrientFront = getVector(0, 0, 1); //FIXME:
    listener.OrientTop = getVector(0, 1, 0); //FIXME:
    listener.Position = getVector(camera.position().x, camera.position().y, camera.position().z);

    X3DAUDIO_EMITTER emitter;
    emitter.pCone = NULL;
    emitter.OrientFront = getVector(0, 0, 1);
    emitter.OrientTop = getVector(0, 1, 0);
    emitter.Velocity = getVector(0, 0, 0); //FIXME:
    emitter.Position = getVector(sceneObject.position().x, sceneObject.position().y, sceneObject.position().z);
    emitter.InnerRadius;
    emitter.InnerRadiusAngle;
    emitter.ChannelCount = 1;
    emitter.ChannelRadius = 0.0f;
    emitter.pChannelAzimuths = NULL; // 1 - channel
    emitter.pVolumeCurve = NULL; // use default
    emitter.pLFECurve = NULL;  // use default
    emitter.pLPFDirectCurve = NULL; // use default
    emitter.pLPFReverbCurve = NULL; // use default
    emitter.pReverbCurve = NULL; // use default
    emitter.CurveDistanceScaler = 100.0f;
    emitter.DopplerScaler = 1.0f;
    emitter.InnerRadius = 2.0f;
    emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;

    DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER;

    X3DAUDIO_DSP_SETTINGS dspSettings;
    FLOAT32 matrixCoefficients[2 * 1];
    dspSettings.pMatrixCoefficients = matrixCoefficients;
    dspSettings.DstChannelCount = 2;
    dspSettings.SrcChannelCount = 1;

    X3DAudioCalculate(_3dAudio, &listener, &emitter, dwCalcFlags, &dspSettings);

    HRESULT hr;
    hr = voice->internal()->SetFrequencyRatio(dspSettings.DopplerFactor);
    if (FAILED(hr)) {
        throw std::runtime_error("Can't set frequency ratio");
    }

    hr = voice->internal()->SetOutputMatrix(_masteringVoice, dspSettings.SrcChannelCount, dspSettings.DstChannelCount, matrixCoefficients);
    if (FAILED(hr)) {
        throw std::runtime_error("Can't set output matrix");
    }
}


}//namespace engine
