#include <LPC213x.h>
#include "ff.h"
#include "spi.h"
#include "wav.h"

#define SPEAKER_PIN (1 << 25)  // P0.10 for speaker

FATFS fs;
FIL file;
UINT br;

void delay_us(unsigned int us) {
    for (volatile unsigned int i = 0; i < us * 10; i++);
}

void play_wav(const char *filename) {
    WAV_HEADER wav;
    BYTE buffer[512];

    if (f_open(&file, filename, FA_READ) != FR_OK) return;
    f_read(&file, &wav, sizeof(WAV_HEADER), &br);
    unsigned int sample_delay = 1000000 / wav.SampleRate;

    while (f_read(&file, buffer, sizeof(buffer), &br) == FR_OK && br > 0) {
        for (int i = 0; i < br; i++) {
            if (buffer[i] > 128) IOSET0 = SPEAKER_PIN;
            else IOCLR0 = SPEAKER_PIN;
            delay_us(sample_delay);
        }
    }

    f_close(&file);
}

int main(void) {
    PINSEL1 &= ~(3 << 20);
    IODIR0 |= SPEAKER_PIN;
    
    if (f_mount(&fs, "", 1) == FR_OK) play_wav("music.wav");
    
    while (1);
}
