#include <unity.h>
#include <output/randomizer.h>
#include <stdlib.h>
#include <stdio.h>

static int size = 10;

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    uint8_t data[size + 2] = {0};
    char message[80];

    TEST_MESSAGE("empty");
    for (uint8_t j = 0; j < size + 2; j++)
    {
        sprintf(message + (j * 4), "%4i", data[j]);
    }
    TEST_MESSAGE(message);

    randomizer.next(data, size);

    TEST_MESSAGE("init");
    for (uint8_t j = 0; j < size+2; j++)
    {
        sprintf(message + (j * 4), "%4i", data[j]);
    }
    TEST_MESSAGE(message);

    for (uint8_t i = 0; i < size; i++)
    {
        uint8_t n = randomizer.next(data, size);
        sprintf(message, "next: %i", n);
        TEST_MESSAGE(message);
        for (uint8_t j = 0; j < size+2; j++)
        {
            sprintf(message + (j * 4), "%4i", data[j]);
        }
        TEST_MESSAGE(message);
    }
    UNITY_END();
}