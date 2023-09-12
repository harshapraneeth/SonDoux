# SonDoux: A Minimalistic C Audio Player

## Description

SonDoux is a straightforward audio player designed with a command-line interface in mind. The primary objective is to enable audio playback in the background while allowing user interaction through non-blocking command-line commands.

## Usage

### Starting the Server

To initiate the server, which runs in the background and listens for incoming commands, use the following command:

```bash
cd sondoux-cli/exe
sondoux -s
```

### Starting the Client

The client process is responsible for instructing the server to create a playlist with audio files located in a specific directory. Use the following command to start the client process:

```bash
sondoux -c "open path/to/audio/"
```

### Available Commands

- **quit:** Terminates both the server and client processes.

- **clear:** Clears the command-line screen.

- **play:** Resumes playback of the currently selected song. If a song is provided as an argument, it creates a playlist containing that song and starts playback.

- **pause:** Pauses the current playback.

- **open:** Opens a directory and creates a playlist with the songs it contains.

- **prev:** Plays the previous song in the playlist if shuffle mode is off; otherwise, plays a random song.

- **next:** Plays the next song in the playlist if shuffle mode is off; otherwise, plays a random song.

- **time:** Displays the elapsed and total time of the current playback.

- **seek:** Skips forward or backward in the audio. Specify an argument of +x or -x to go forward or backward by x seconds. If no sign is provided, it seeks to the x-second mark in the audio.

- **shuffle:** Toggles shuffle mode on or off. With shuffle mode enabled, a random song from the playlist is played when the current song finishes.

- **repeat:** Toggles repeat mode on or off. With repeat mode enabled, the currently playing audio restarts once it finishes.

- **autoplay:** Controls autoplay behavior. Use `autoplay off` to prevent the next song from automatically playing.

- **toggle:** Allows toggling of play, shuffle, repeat, and autoplay options.

- **scroll:** When creating a playlist, some songs are displayed. Use `scroll +/-x` to scroll up or down in the list. If no sign is provided, it jumps to a specific song in the playlist.

- **jump:** Jumps to a particular song in the playlist. If `jump x` begins with a sign `jump +/-x`, it plays the next or previous x-th song in the playlist.