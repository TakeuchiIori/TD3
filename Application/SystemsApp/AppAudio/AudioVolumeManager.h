#pragma once
#include "Systems/Audio/Audio.h"
#include <list>

enum AudioType
{
	kBGM,
	kSE,
	kAudioTypeNum,
};

class AudioVolumeManager
{
public:
	static AudioVolumeManager* GetInstance() {
		static AudioVolumeManager instance;
		return &instance;
	}

	~AudioVolumeManager();

	void Initialize();

	void Finalize();

public:
	void SetSourceToSubmix(IXAudio2SourceVoice* pSourceVoice, AudioType audioType = kSE);

	void SetVolume(AudioType audioType, float volume);

private:
	void ConnectSourceToSubmix(IXAudio2SubmixVoice* submixVoice, IXAudio2SourceVoice* pSourceVoice);


private:
	IXAudio2* xAudio2_ = nullptr;

	IXAudio2SubmixVoice* BGMSubmixVoice_ = nullptr;
	IXAudio2SubmixVoice* SESubmixVoice_ = nullptr;

	std::list<IXAudio2SourceVoice*> pSourceVoices_;
};
