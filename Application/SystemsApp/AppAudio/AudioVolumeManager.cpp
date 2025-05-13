#include "AudioVolumeManager.h"

AudioVolumeManager::~AudioVolumeManager()
{
}

void AudioVolumeManager::Initialize()
{
	xAudio2_ = Audio::GetInstance()->GetXAudio2();

	xAudio2_->CreateSubmixVoice(&BGMSubmixVoice_, 2, 44100);
	xAudio2_->CreateSubmixVoice(&SESubmixVoice_, 2, 44100);
	xAudio2_->CreateSubmixVoice(&UISoundSubmixVoice_, 2, 44100);
}

void AudioVolumeManager::Finalize()
{
	for (IXAudio2SourceVoice*& voice : pSourceVoices_) {
		if (voice) {
			Audio::GetInstance()->PauseAudio(voice);
			voice->DestroyVoice();
			voice = nullptr;
		}
	}
	pSourceVoices_.clear();

	if (BGMSubmixVoice_)
	{
		BGMSubmixVoice_->DestroyVoice();
		BGMSubmixVoice_ = nullptr;
	}

	if (SESubmixVoice_)
	{
		SESubmixVoice_->DestroyVoice();
		SESubmixVoice_ = nullptr;
	}

	if (UISoundSubmixVoice_)
	{
		UISoundSubmixVoice_->DestroyVoice();
		UISoundSubmixVoice_ = nullptr;
	}
}

void AudioVolumeManager::SetSourceToSubmix(IXAudio2SourceVoice* pSourceVoice, AudioType audioType)
{
	pSourceVoices_.push_back(pSourceVoice);
	switch (audioType)
	{
	case kBGM:
		ConnectSourceToSubmix(BGMSubmixVoice_, pSourceVoice);
		break;

	case kSE:
		ConnectSourceToSubmix(SESubmixVoice_, pSourceVoice);
		break;

	case kUISound:
		ConnectSourceToSubmix(UISoundSubmixVoice_, pSourceVoice);
		break;
	case kMaster:
	default:
		break;
	}
}

void AudioVolumeManager::SetVolume(AudioType audioType, float volume)
{
	switch (audioType)
	{
	case kBGM:
		BGMSubmixVoice_->SetVolume(volume);
		break;

	case kSE:
		SESubmixVoice_->SetVolume(volume);
		break;

	case kUISound:
		UISoundSubmixVoice_->SetVolume(volume);
		break;
	case kMaster:
	default:
		Audio::GetInstance()->SetMasterVolume(volume);
		break;
	}
}

void AudioVolumeManager::ConnectSourceToSubmix(IXAudio2SubmixVoice* submixVoice, IXAudio2SourceVoice* pSourceVoice)
{
	XAUDIO2_VOICE_SENDS sendList = {};
	XAUDIO2_SEND_DESCRIPTOR sendDesc = {};

	sendDesc.pOutputVoice = submixVoice;
	sendDesc.Flags = 0;

	sendList.SendCount = 1;
	sendList.pSends = &sendDesc;

	HRESULT hr = pSourceVoice->SetOutputVoices(&sendList);
	assert(SUCCEEDED(hr));

}
