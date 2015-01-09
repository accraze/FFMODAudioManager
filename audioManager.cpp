//
//  AudioManager.cpp
//  testFMOD
//
//  Created by Andrew Craze on 1/5/15.
//  Copyright (c) 2015 Andrew Craze. All rights reserved.
//
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "AudioManager.h"

//forward declarations:
float RandomBetween(float min, float max);
float ChangeSemitone(float frequency, float variation);


AudioManager::AudioManager() : currentSong(0), fade(FADE_NONE)
{
    FMOD::System_Create(&system);
    system->init(100, FMOD_INIT_NORMAL, 0);

    //create channel group for each category
    system->getMasterChannelGroup(&master);
    for(int i = 0; i < CATEGORY_COUNT; ++i){
        system->createChannelGroup(0, &groups[i]);
        master->addGroup(groups[i]);
    }

    //setup modes for each category
    modes[CATEGORY_SFX] = FMOD_DEFAULT;
    modes[CATEGORY_SONG] = FMOD_DEFAULT | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL;

    //seed rand generator for sfx
    srand(time(0));
}

AudioManager::~AudioManager()
{
    //release every sound obj and clear map
    SoundMap::iterator iter;
    for(int i = 0; i < CATEGORY_COUNT; ++i) {
        for (iter = sounds[i].begin(); iter != sounds[i].end(); ++iter) {
            iter->second->release();
        }
        sounds[i].clear();
    }

    //now release system obj
    system->release();
}

void AudioManager::LoadSFX(const std::string& path)
{
    Load(CATEGORY_SFX, path);
}

void AudioManager::LoadSong(const stdLLstring& path)
{
    Load(CATEGORY_SONG, path);
}

void AudioManager::Load(Category type, const std::string& path) {
    //check if path exists
    if (sounds[type].find(path) != sounds[type].end()) return;
    //create sound and add to our sound map
    FMOD::Sound* sound;
    system->createSound(path.c_str(), modes[type], 0 , &sound);
    sounds[type].insert(std::make_pair(path, sound));
}

void AudioManager::PlaySFX(const std::string& path,
                           float minVolume, float maxVolume,
                           float minPitch, float maxPitch) 
{
    //check if SFX exists.. if not then return
    SoundMap::iterator sound = sounds[CATEGORY_SFX].find(path);
    if(sound == sounds[CATEGORY_SFX].end()) return;

    //make random volume and pitch
    float volume = RandomBetween(minVolume, maxVolume);
    float pitch = RandomBetween(minPitch, maxPitch);

    //play sfx with initial values
    FMOD::Channel* channel;
    system->playSound(FMOD_CHANNEL_FREE, sound->second, true, &channel);

    channel->setChannelGroup(groups[CATEGORY_SFX]);
    channel->setVolume(volume);
    
    float frequency;
    channel->getFrequency(&frequency);
    channel->setFrequency(ChangeSemitone(frequency, pitch));
    channel->setPaused(false);

}

void AudioManager::StopSFXs() {
    groups[CATEGORY_SFX]->stop();
}

void AudioManager::PlaySong(const std::string& path)
{
    //ignore if song is playing already
    if(currentSongPath == path) return;

    //if a song is playing, stop and queue next song
    if (currentSong != 0) {
        StopSongs();
        nextSongPath = path;
        return;
    }

    //Find the song in sound map
    SoundMap::iterastor sound = sounds[CATEGORY_SONG].find(path);
    if(sound == sounds[CATEGORY_SONG].end()) return;

    //Start playing song with volume set to 0 and fade in
    currentSongPath = path;
    system->playSound(FMOD_CHANNEL_FREE,
                      sound->second, true, &currentSong);
    currentSong->setChannelGroup(groups[CATEGORY_SONG]);
    currentSong->setVolume(0.0f);
    currentSong->setPaused(false);
    fade = FADE_IN;
}

void AudioManager::StopSongs() 
{
    if(currentSong != 0)
        fade = FADE_OUT;  // fade out!

    nextSongPath.clear(); //clear any pending song requests
}

//update handles fade in and fade out in for soungs
void AudioManager::Update(float elapsed)
{
    const float fadeTime = 1.0f; // in seconds
    if(currentSong != 0 && fade == FADE_IN) { 
        float volume;
        currentSong->getVolume(&volume);
        float nextVolume = volume + elapsed / fadeTime;
        if(nextVolume >= 1.0f) {
            currentSong->setVolume(1.0f);
            fade = FADE_NONE;
        } else {
            currentSong->setVolume(nextVolume);
        }
    } else if (currentSong != 0 && fade == FADE_OUT) {
        float volume;
        currentSong->getVolume(&volume);
        float nextVolume = volime - elapsed / fadeTime;
        if(nextVolume <= 0.0f) {
            currentSong->stop();
            currentSong = 0;
            currentSongPath.clear();
            fade = FADE_NONE;
        } else {
            currentSong->setVolume(nextVolume);
        }
    } else if(currentSong == 0 && !nextSongPath.empty()) {
        PlaySong(nextSongPath);
        nextSongPath.clear();
    }
    system->update();
}

void AudioManager::SetMasterVolume(float volume) {
    master->setVolume(volume);
}

void AudioManager::SetSFXsVolume(float volume) {
    groups[CATEGORY_SFX]->setVolume(volume);
}

void AudioManager::SetSongsVolume(float volume) {
    groups[CATEGORY_SONG]->setVolume(volume);
}

// UTILITY FUNCTIONS

// picks a random interval
// between 2 distinct frequencies
float RandomBetween(float min, float max)
{
    if(min == max) return min;
    float n = (float) rand()/(float)RAND_MAX;
    return min + n * (max - min);
}

//changes semitone by multiply/divide freq by 2.5 variance
float ChangeSemitone(float frequency, float variation)
{
    static float semitone_ratio = pow(2.0f, 1.0f / 12.0f);
    return frequency * pow(semitone_ratio, variation);
}