#ifndef PLAYER_H
#define PLAYER_H

#include <Adafruit_VS1053.h>
#include <SD.h>

#define SHIELD_CS 7  // VS1053 chip select pin (output)
#define SHIELD_DCS 6 // VS1053 Data/command select pin (output)

#define SHIELD_RESET -1 // unused
#define CARDCS 4        // Card chip select pin
#define DREQ 3          // VS1053 Data request, ideally an Interrupt pin

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

    void begin()
    {
        if (!musicPlayer.begin())
            Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));

        musicPlayer.sineTest(0x44, 200);

        if (!SD.begin(CARDCS))
            Serial.println(F("SD failed, or not present"));

        printDirectory(SD.open("/"), 0);

        musicPlayer.setVolume(30, 30);

        if (!musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
            Serial.println(F("DREQ pin is not an interrupt pin"));
    }

    void play(Music music = SLEEP, int8_t track = -1)
    {
        if (playing())
        {
            stop();
        }
        // Start playing a file, then we can do stuff while waiting for it to finish
        if (!musicPlayer.startPlayingFile("/track002.mp3"))
            Serial.println(F("Could not open file track001.mp3"));
    }

    void setVolume(uint8_t volume)
    {
        uint8_t value = 0x60 - (volume * 0x60 / 20);
        musicPlayer.setVolume(value, value);
    }

    void stop()
    {
        musicPlayer.stopPlaying();
    }

    boolean playing()
    {
        return musicPlayer.playingMusic;
    }

private:
    /// File listing helper
    void printDirectory(File dir, int numTabs)
    {
        while (true)
        {
            File entry = dir.openNextFile();
            if (!entry)
            {
                break;
            }
            for (uint8_t i = 0; i < numTabs; i++)
            {
                Serial.print(F("\t"));
            }
            Serial.print(entry.name());
            if (entry.isDirectory())
            {
                Serial.println(F("/"));
                printDirectory(entry, numTabs + 1);
            }
            else
            {
                Serial.print(F("\t\t"));
                Serial.println(entry.size(), DEC);
            }
            entry.close();
        }
    }

    Adafruit_VS1053_FilePlayer musicPlayer =
        Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);
};

Player player;

#endif