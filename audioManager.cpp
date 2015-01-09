//
//  AudioManager.cpp
//  testFMOD
//
//  Created by Andrew Craze on 1/5/15.
//  Copyright (c) 2015 Andrew Craze. All rights reserved.
//

#include "AudioManager.h"

AudioManager::AudioManager()
{
    FMOD::System_Create(&system);
    system->init(100, FMOD_INIT_NORMAL, 0);
}

AudioManager::~AudioManager()
{
    //release every sound obj and clear map
    SoundMap::iterator iter;
    
    for (iter = sounds.begin(); iter != sounds.end(); ++iter) {
        iter->second->release();
    }
    sounds.clear();
    
    //now release system obj
    system->release();
    system = 0;
}

void AudioManager::Update(float elapsed)
{
    system->update();
}

void AudioManager::LoadOrStream(const std::string& path, bool stream)
{
    //check if already loaded...
    if(sounds.find(path) != sounds.end()) return;
    
    //create sound obj
    FMOD::Sound* sound;
    if(stream)
        system->createStream(path.c_str(), FMOD_DEFAULT, 0, &sound);
    else
        system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);
    
    //save sound obj to map
    sounds.insert(std::make_pair(path, sound));
}

void AudioManager::Load(const std::string& path)
{
    LoadOrStream(path, false);
}

void AudioManager::Stream(const std::string& path)
{
    LoadOrStream(path, true);
}

void AudioManager::Play(const std::string& path)
{
    //check if sound exists.. ignore if DNE
    SoundMap::iterator sound = sounds.find(path);
    if (sound == sounds.end()) return;
    
    //play sound
    FMOD::Channel *channel = NULL;
    system->playSound(sound->second, NULL, false, &channel);
    
}

