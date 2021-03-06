/**********************************************************************************************
*
*   Ngine - The 2D game engine.
*
*   Copyright (C) 2019 NerdThings
*
*   LICENSE: Apache License 2.0
*   View: https://github.com/NerdThings/Ngine/blob/master/LICENSE
*
**********************************************************************************************/

#include "Sound.h"

#include <miniaudio.h>

#include "AudioDevice.h"

namespace NerdThings::Ngine::Audio {
    // Destructor

    Sound::~Sound() {
        Unload();
    }

    // Public Methods

    bool Sound::IsPlaying() const {
        return Stream.Buffer->IsPlaying();
    }

    bool Sound::IsValid() const {
        return Stream.Buffer != nullptr;
    }

    Sound *Sound::LoadSound(const Filesystem::Path &path_) {
        auto wav = Wave::LoadWave(path_);

        auto snd = LoadSoundFromWave(wav);

        delete wav;

        return snd;
    }

    Sound *Sound::LoadSoundFromWave(Wave *wave_) {
        if (wave_->IsValid()) {
            auto snd = new Sound();

            // We convert every sound to be the same format.
            ma_format formatIn = ((wave_->SampleSize == 8) ? ma_format_u8 : ((wave_->SampleSize == 16) ? ma_format_s16 : ma_format_f32));
            ma_uint32 frameCountIn = wave_->SampleCount/wave_->Channels;

            ma_uint32 frameCount = (ma_uint32)ma_convert_frames(nullptr, DEVICE_FORMAT, DEVICE_CHANNELS, DEVICE_SAMPLE_RATE, nullptr, formatIn, wave_->Channels, wave_->SampleRate, frameCountIn);
            if (frameCount == 0) ConsoleMessage("Failed to get frame count for format conversion!", "WARN", "Sound");

            auto audioBuffer = AudioDevice::InitAudioBuffer(DEVICE_FORMAT, DEVICE_CHANNELS, DEVICE_SAMPLE_RATE, frameCount, BUFFER_USAGE_STATIC);
            if (audioBuffer == nullptr) ConsoleMessage("Failed to create audio buffer!", "WARN", "Sound");

            frameCount = (ma_uint32)ma_convert_frames(audioBuffer->Buffer, audioBuffer->DSP.formatConverterIn.config.formatIn, audioBuffer->DSP.formatConverterIn.config.channels, audioBuffer->DSP.src.config.sampleRateIn, wave_->Data, formatIn, wave_->Channels, wave_->SampleRate, frameCountIn);
            if (frameCount == 0) ConsoleMessage("Format conversion failed!", "WARN", "Sound");

            snd->SampleCount = frameCount*DEVICE_CHANNELS;
            snd->Stream.SampleRate = DEVICE_SAMPLE_RATE;
            snd->Stream.SampleSize = 32;
            snd->Stream.Channels = DEVICE_CHANNELS;
            snd->Stream.Buffer = audioBuffer;

            return snd;
        }

        return nullptr;
    }

    void Sound::Pause() {
        Stream.Buffer->Pause();
    }

    void Sound::Play() {
        Stream.Buffer->Play();
    }

    void Sound::Resume() {
        Stream.Buffer->Resume();
    }

    void Sound::SetPitch(float pitch_) {
        Stream.Buffer->SetPitch(pitch_);
    }

    void Sound::SetVolume(float vol_) {
        Stream.Buffer->SetVolume(vol_);
    }

    void Sound::Stop() {
        Stream.Buffer->Stop();
    }

    void Sound::Unload() {
        // Close buffer
        AudioDevice::CloseAudioBuffer(Stream.Buffer);
        Stream.Buffer = nullptr;
    }
}
