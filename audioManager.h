//
//  AudioManager.h
//  testFMOD
//
//  Created by Andrew Craze on 1/5/15.
//  Copyright (c) 2015 Andrew Craze. All rights reserved.
//

#ifndef testFMOD_AudioManager_h
#define testFMOD_AudioManager_h

#include <string>
#include <map>
#include <fmod>

typedef std::map<std::string, FMOD::Sound*> SoundMap;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    void Update(float elapsed);
    void Load(const std::string& path);
    void Stream(const std::string& path);
    void Play(const std::string& path);
    
private:
    void LoadOrStream(const std::string& path, bool stream);
    FMOD::System* system;
    SoundMap sounds;
};

#endif // AudioManager_h_

