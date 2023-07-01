#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <stdlib.h>

#define SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

class Randomizer
{
public:
    uint8_t next(uint8_t *data, uint8_t size)
    {
        uint8_t *current = data + 1;
        uint8_t *sequence = data + 2;
        if (size != data[0])
        {
            for (uint8_t i = 0; i < size; i++)
            {
                sequence[i] = i;
                if (i > 1)
                {
                    randomSwap(sequence, i, 0, i - 1, size);
                }
            }
            *current = 0;
            data[0] = size;
        }
        else
        {
            randomSwap(sequence, *current, *current + size / 2, *current + size - 1, size);
            *current = (*current + 1) % size;
        }

        return sequence[*current];
    };

private:
    void randomSwap(uint8_t *values, uint8_t pos, uint8_t from, uint8_t to, uint8_t size)
    {
        uint8_t other = (from + random(to - from + 1)) % size;
        SWAP(values[pos], values[other]);
    }
};

Randomizer randomizer;

#endif