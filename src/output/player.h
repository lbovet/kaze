#ifndef PLAYER_H
#define PLAYER_H
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <Adafruit_VS1053.h>
#include <SD.h>
#include <data/storage.h>
#include <output/randomizer.h>

#define SHIELD_CS 7  // VS1053 chip select pin (output)
#define SHIELD_DCS 6 // VS1053 Data/command select pin (output)

#define SHIELD_RESET -1 // unused
#define CARDCS 4        // Card chip select pin
#define DREQ 3          // VS1053 Data request, ideally an Interrupt pin

#define MAX_TRACK_COUNT 16
#define DATA_SIZE MAX_TRACK_COUNT + 2

class Player
{
public:
    enum Music
    {
        SLEEP,
        MASSAGE,
        LOVE,
        ALARM
    };
#define MUSIC_COUNT ALARM + 1

    void begin()
    {
        if (!musicPlayer.begin())
            Serial.println(F("E VS1053"));

        if (!SD.begin(CARDCS))
            Serial.println(F("E SD"));

        musicPlayer.setVolume(20, 20);

        if (!musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
            Serial.println(F("E DREQ"));

        blup();
    }

    void play(Music music)
    {
        if (playing())
        {
            stop();
        }

        char path[22];
        if (!readNextFilename(music, path))
        {
            Serial.println(F("E Next"));
            failing = true;
            return;
        }

        Serial.print(F("Playing "));
        Serial.println(path);
        if (!musicPlayer.startPlayingFile(path))
        {
            Serial.println(F("E Play"));
            failing = true;
        }
        failing = false;
    }

    void setVolume(uint8_t volume)
    {
        uint8_t value = 0x60 - (volume * 0x60 / 20);
        musicPlayer.setVolume(value, value);
    }

    void stop()
    {
        musicPlayer.stopPlaying();
        delay(50);
    }

    boolean playing()
    {
        return musicPlayer.playingMusic;
    }

    void signal()
    {
        musicPlayer.setVolume(5, 5);
        playFile("/sound/signal.mid");
    }

    void blup()
    {
        musicPlayer.setVolume(0, 0);
        playFile("/sound/blup.mid");
    }

    void fallBackAlarm()
    {
        if(failing) {
            musicPlayer.sineTest(0x44, 500);
        }
    }

private:
    int readNextFilename(Music music, char *path)
    {
        char *dirName;
        switch (music)
        {
        case (ALARM):
            dirName = "ALARM";
            break;
        case (SLEEP):
            dirName = "SLEEP";
            break;
        case (MASSAGE):
            dirName = "MASSAGE";
            break;
        case (LOVE):
        default:
            dirName = "LOVE";
            break;
        }
        File dir = SD.open(dirName);
        if (!dir)
        {
            Serial.print(F("E Dir: "));
            Serial.println(dirName);
            return 0;
        }
        storage.read(TRACK, music * DATA_SIZE, sequenceData, DATA_SIZE);
        uint8_t next = randomizer.next(sequenceData, countFiles(dir));
        storage.write(TRACK, music * DATA_SIZE, sequenceData, DATA_SIZE);

        strcpy(path, "/");
        strcat(path, dirName);
        strcat(path, "/");
        dir.rewindDirectory();
        appendFileName(dir, next, path);
        dir.close();
        return 1;
    }

    uint8_t countFiles(File dir)
    {
        if (!dir)
        {
            return 0;
        }
        uint8_t result = 0;
        while (true)
        {
            File entry = dir.openNextFile();
            if (!entry)
            {
                break;
            }
            if (!entry.isDirectory())
            {
                result++;
            }
            entry.close();
        }
        return result;
    }

    void appendFileName(File dir, uint8_t fileNumber, char *path)
    {
        uint8_t i = 0;
        do
        {
            File entry = dir.openNextFile();
            if (!entry)
            {
                break;
            }
            if (!entry.isDirectory())
            {
                if (i == fileNumber)
                {
                    strcat(path, entry.name());
                }
                i++;
            }
            entry.close();
        } while (i <= fileNumber);
    }

    void playFile(char *path)
    {
        if (playing())
        {
            stop();
        }
        if (!musicPlayer.startPlayingFile(path))
        {
            Serial.print(F("E Play: "));
            Serial.println(path);
        }
    }

    Adafruit_VS1053_FilePlayer musicPlayer =
        Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);
    byte sequenceData[DATA_SIZE];
    boolean failing =false;
    boolean beep = false;
};

Player player;

#endif