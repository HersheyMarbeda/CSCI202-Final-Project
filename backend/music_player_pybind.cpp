#include <pybind11/pybind11.h>
#include "music_player.h"

namespace py = pybind11;

PYBIND11_MODULE(music_player, m) {
    py::class_<MusicPlayer>(m, "MusicPlayer")
        .def(py::init<>())
        .def("play", &MusicPlayer::play)
        .def("pause", &MusicPlayer::pause)
        .def("stop", &MusicPlayer::stop)
        .def("queueFolder", &MusicPlayer::queueFolder)
        .def("queueSong", &MusicPlayer::queueSong)
        .def("setVolume", &MusicPlayer::setVolume)
        .def("getVolume", &MusicPlayer::getVolume)
        .def("getSongDuration", &MusicPlayer::getSongDuration)
        .def("getCurrentTime", &MusicPlayer::getCurrentTime);  
}