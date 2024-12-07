#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <queue>
#include <string>
#include <SFML/Audio.hpp>

class MusicPlayer {
public:
    MusicPlayer();
    void play();
    void pause();
    void stop();
    void queueFolder(const std::string& folderPath);
    void queueSong(const std::string& songPath);
    void setVolume(int newVolume);
    int getVolume() const;
    std::string getSongDuration(const std::string& songPath);
    std::string getCurrentTime() const;  

private:
    std::queue<std::string> songQueue;
    int volume;
    bool isPlaying;
    bool isPaused;
    sf::SoundBuffer buffer;
    sf::Sound sound;
};

#endif // MUSIC_PLAYER_H