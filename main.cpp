//
//  main.cpp
//  audioManager
//
//  Created by Andrew Craze on 1/6/15.
//  Copyright (c) 2015 Andrew Craze. All rights reserved.
//

#include <iostream>
#include <SFML/Window>
#include "audioManager.h"

int main(int argc, const char * argv[]) {
    sf::Window window (sf::VideoMode(320,240), "Audio Playback");
    sf::Clock clock;
    
    // initialize
    AudioManager audio;
    audio.Load("explosion.wav");

    //game loop
    while (window.isOpen()) {
      //60 frames per second
      float elapsed = clock.getElapsedTime().asSeconds();
      if(elapsed < 1.0f / 60.0f) continue;
      clock.restart();
      sf::Event event;
      while (window.pollEvent (event)) {
        //handle Window Events
        if(event.type == sf::Event::Closed)
          window.close();

        //handle user input
        if(event.type ==sf::Event::KeyPressed && 
           event.key.coce == sf::Keyboard::Space)
          audio.Play("explosion.wav");
      }

      audio.Update(elapsed);
    }
    
    return 0;
}
