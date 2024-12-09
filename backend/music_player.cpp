#include "music_player.h"
#include <iostream>
#include <filesystem>
#include <SFML/Audio.hpp>

namespace fs = std::filesystem;

MusicPlayer::MusicPlayer() : volume(50), isPlaying(false), isPaused(false) {}

void MusicPlayer::play() {
    if (isPaused) {
        sound.play();
        std::cout << "Resuming playback." << std::endl;
        isPaused = false;
        isPlaying = true;
    } else if (songQueue.empty()) {
        std::cout << "Queue is empty. Nothing to play." << std::endl;
    } else {
        std::string songPath = songQueue.front();
        if (!buffer.loadFromFile(songPath)) {
            std::cerr << "Error: Could not load song." << std::endl;
            return;
        }
        sound.setBuffer(buffer);
        sound.setVolume(volume);
        sound.play();
        currentSongTitle = fs::path(songPath).stem().string();  // Store the current song title
        std::cout << "Playing: " << songPath << " at volume " << volume << "%" << std::endl;
        songQueue.pop();  // Remove the song from the queue after playing
        isPlaying = true;
    }
}

void MusicPlayer::pause() {
    if (isPlaying) {
        sound.pause();
        std::cout << "Playback paused." << std::endl;
        isPlaying = false;
        isPaused = true;
    } else {
        std::cout << "Playback is already paused." << std::endl;
    }
}

void MusicPlayer::stop() {
    if (isPlaying || isPaused) {
        sound.stop();
        std::cout << "Playback stopped." << std::endl;
        isPlaying = false;
        isPaused = false;
        currentSongTitle.clear();  // Clear the current song title
    } else {
        std::cout << "Playback is already stopped." << std::endl;
    }
}

void MusicPlayer::queueFolder(const std::string& folderPath) {
    if (!fs::exists(folderPath)) {
        std::cerr << "Error: Folder does not exist." << std::endl;
        return;
    }

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
            songQueue.push(entry.path().string());
            std::cout << "Queued: " << entry.path().filename().string() << std::endl;
        }
    }
}

void MusicPlayer::queueSong(const std::string& songPath) {
    if (fs::exists(songPath) && fs::path(songPath).extension() == ".mp3") {
        songQueue.push(songPath);
        std::cout << "Queued: " << songPath << std::endl;
        if (!isPlaying && !isPaused) {
            play();
        }
    } else {
        std::cerr << "Error: Song does not exist or is not an mp3 file: " << songPath << std::endl;
    }
}

void MusicPlayer::setVolume(int newVolume) {
    if (newVolume < 0 || newVolume > 100) {
        std::cerr << "Error: Volume must be between 0 and 100." << std::endl;
        return;
    }
    volume = newVolume;
    sound.setVolume(volume);
    std::cout << "Volume set to " << volume << "%" << std::endl;
}

int MusicPlayer::getVolume() const {
    return volume;
}

std::string MusicPlayer::getSongDuration(const std::string& songPath) {
    sf::Music music;
    if (!music.openFromFile(songPath)) {
        return "Error";
    }
    sf::Time duration = music.getDuration();
    int minutes = static_cast<int>(duration.asSeconds()) / 60;
    int seconds = static_cast<int>(duration.asSeconds()) % 60;
    return std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
}

std::string MusicPlayer::getCurrentTime() const {
    sf::Time currentTime = sound.getPlayingOffset();
    int minutes = static_cast<int>(currentTime.asSeconds()) / 60;
    int seconds = static_cast<int>(currentTime.asSeconds()) % 60;
    return std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
}

std::string MusicPlayer::getCurrentSongTitle() const {
    return currentSongTitle;
}

void MusicPlayer::update() {
    if (isPlaying && sound.getStatus() == sf::Sound::Stopped) {
        isPlaying = false;
        if (!songQueue.empty()) {
            play();
        }
    }
}

bool MusicPlayer::isSongFinished() const {
    return sound.getStatus() == sf::Sound::Stopped;
}