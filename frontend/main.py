import sys
import os
import ctypes
import glob
from mutagen.mp3 import MP3
from mutagen.id3 import ID3, APIC

# Add the backend build directory to Python's module path
lib_path = os.path.abspath("../backend/build/music_player.so")
ctypes.CDLL(lib_path)
sys.path.append(os.path.abspath("../backend/build"))

from music_player import MusicPlayer  # Import the PyBind11 module

from PyQt5.QtWidgets import QApplication, QVBoxLayout, QHBoxLayout, QPushButton, QListWidget, QWidget, QLabel, QSlider
from PyQt5.QtCore import Qt, QTimer  # Correct import for QTimer
from PyQt5.QtGui import QPixmap  # Correct import for QPixmap
from pathlib import Path

class MusicApp(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Music Player")
        self.player = MusicPlayer()

        self.layout = QVBoxLayout()

        # Add a label for the list of songs
        songs_label = QLabel("List of Songs")
        self.layout.addWidget(songs_label)

        self.song_list = QListWidget()
        self.load_songs()
        self.layout.addWidget(self.song_list)

        # Add a label for the queued songs
        queued_songs_label = QLabel("Queued Songs")
        self.layout.addWidget(queued_songs_label)

        self.queued_songs_list = QListWidget()
        self.layout.addWidget(self.queued_songs_list)

        button_layout = QHBoxLayout()

        self.play_button = QPushButton("Play")
        self.play_button.clicked.connect(self.play_song)
        button_layout.addWidget(self.play_button)

        self.pause_button = QPushButton("Pause")
        self.pause_button.clicked.connect(self.pause_song)
        button_layout.addWidget(self.pause_button)

        self.stop_button = QPushButton("Stop")
        self.stop_button.clicked.connect(self.stop_song)
        button_layout.addWidget(self.stop_button)

        button_layout.addStretch()  # Add a stretch to push the Queue button to the far right

        self.queue_button = QPushButton("Queue")
        self.queue_button.clicked.connect(self.queue_song)
        button_layout.addWidget(self.queue_button)

        self.layout.addLayout(button_layout)

        self.volume_slider = QSlider(Qt.Horizontal)
        self.volume_slider.setRange(0, 100)
        self.volume_slider.setValue(50)
        self.volume_slider.valueChanged.connect(self.set_volume)
        self.layout.addWidget(self.volume_slider)

        self.volume_label = QLabel("Volume: 50%")
        self.layout.addWidget(self.volume_label)

        self.current_time_label = QLabel("Current Time: 0:00")
        self.layout.addWidget(self.current_time_label)

        self.cover_label = QLabel()
        self.layout.addWidget(self.cover_label)

        self.setLayout(self.layout)

        # Set up a timer to update the current playback time
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_current_time)
        self.timer.start(1000)  # Update every second

    def load_songs(self):
        songs_dir = os.path.abspath("../songs")
        mp3_files = glob.glob(os.path.join(songs_dir, "*.mp3"))
        for mp3 in mp3_files:
            song_title = Path(mp3).stem
            duration = self.player.getSongDuration(mp3)
            self.song_list.addItem(f"{song_title} ({duration})")

    def play_song(self):
        self.player.play()
        self.update_cover_image()

    def pause_song(self):
        self.player.pause()

    def stop_song(self):
        self.player.stop()
        if self.queued_songs_list.count() > 0:
            self.queued_songs_list.takeItem(0)

    def queue_song(self):
        selected_items = self.song_list.selectedItems()
        if selected_items:
            song_title = selected_items[0].text().split(" (")[0]  # Extract the song title without duration
            song_path = os.path.abspath(f"../songs/{song_title}.mp3")
            self.queued_songs_list.addItem(song_title)
            self.player.queueSong(song_path)

    def set_volume(self):
        volume = self.volume_slider.value()
        self.player.setVolume(volume)
        self.volume_label.setText(f"Volume: {volume}%")

    def update_current_time(self):
        current_time = self.player.getCurrentTime()
        self.current_time_label.setText(f"Current Time: {current_time}")

    def update_cover_image(self):
        selected_items = self.song_list.selectedItems()
        if selected_items:
            song_title = selected_items[0].text().split(" (")[0]  # Extract the song title without duration
            song_path = os.path.abspath(f"../songs/{song_title}.mp3")
            audio = MP3(song_path, ID3=ID3)
            for tag in audio.tags.values():
                if isinstance(tag, APIC):
                    cover_data = tag.data
                    pixmap = QPixmap()
                    if pixmap.loadFromData(cover_data):
                        self.cover_label.setPixmap(pixmap)
                        print(f"Cover image loaded for {song_title}")
                    else:
                        print(f"Failed to load cover image for {song_title}")
                    break
            else:
                print(f"No cover image found for {song_title}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MusicApp()
    window.show()
    sys.exit(app.exec_())