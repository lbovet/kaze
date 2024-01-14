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

const char SLEEP_STRING[] PROGMEM = "SLEEP";
const char MASSAGE_STRING[] PROGMEM = "MASSAGE";
const char LOVE_STRING[] PROGMEM = "LOVE";
const char ALARM_STRING[] PROGMEM = "ALARM";
const char SIGNAL_STRING[] PROGMEM = "/sound/signal.mid";
const char BLUP_STRING[] PROGMEM = "/sound/blup.mid";

// Playing time (minutes)
#define SHORT 10
#define MEDIUM 60
#define LONG 15*60

const char *const STRINGS[] PROGMEM = {
    SLEEP_STRING,
    MASSAGE_STRING,
    LOVE_STRING,
    ALARM_STRING,
    SIGNAL_STRING,
    BLUP_STRING
};

#define SIGNAL_OFFSET 4
#define BLUP_OFFSET 5

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

    void play(Music music, long minutesToPlay = 0)
    {
        if(minutesToPlay == 0) {
            switch(music) {
                case ALARM:
                    minutesToPlay = SHORT;
                    break;
                case SLEEP:
                    minutesToPlay = MEDIUM;
                    break;
                case LOVE:
                case MASSAGE:
                    minutesToPlay = LONG;
                    break;
            }
        }
        if (minutesToPlay > 0)
        {
            chrono.restart();
            timeToPlay = minutesToPlay * 60 * 1000;
        }

        if (playing())
        {
            stop();
        }

        currentMusic = music;

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

    void update() {
        if(timeToPlay > 0) {
            if(playing()) {
                if(chrono.elapsed() > timeToPlay) {
                    timeToPlay = 0;
                }
            } else {
                if(chrono.elapsed() < timeToPlay) {
                    play(currentMusic, -1);
                }
            }
        }
    }

    void setVolume(uint8_t volume)
    {
        uint8_t value = 0x60 - (volume * 0x60 / 20);
        musicPlayer.setVolume(value, value);
    }

    void stop()
    {
        timeToPlay = 0;
        musicPlayer.stopPlaying();
        delay(50);
    }

    boolean playing()
    {
        return musicPlayer.playingMusic;
    }

    void signal()
    {
        char path[22];
        strcpy_P(path, (char *)pgm_read_word(&(STRINGS[SIGNAL_OFFSET])));
        musicPlayer.setVolume(5, 5);
        playFile(path);
    }

    void blup()
    {
        char path[22];
        strcpy_P(path, (char *)pgm_read_word(&(STRINGS[BLUP_OFFSET])));
        musicPlayer.setVolume(0, 0);
        playFile(path);
    }

    void fallBackAlarm()
    {
        if (failing)
        {
            musicPlayer.sineTest(0x44, 500);
        }
    }

private:
    int readNextFilename(Music music, char *path)
    {
        char dirName[9];
        strcpy_P(dirName, (char *)pgm_read_word(&(STRINGS[music])));
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
    boolean failing = false;
    boolean beep = false;
    Chrono chrono;
    Music currentMusic;
    unsigned long timeToPlay = 0;
};

Player player;

#endif