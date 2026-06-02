#include "lpc213x.h"
#include "stdint.h"

// WS2812 Configuration
#define WS2812_PIN (1 << 10)  // Use P0.10 for WS2812 data line
#define NUM_LEDS 10           // Number of LEDs in the strip

// WS2812 Timing Requirements (in microseconds)
#define T0H_TIME 0.35f        // High time for "0" bit (0.35 탎)
#define T0L_TIME 0.8f         // Low time for "0" bit (0.8 탎)
#define T1H_TIME 0.7f         // High time for "1" bit (0.7 탎)
#define T1L_TIME 0.6f         // Low time for "1" bit (0.6 탎)
#define RESET_TIME 50.0f      // Reset time (>50 탎)

// Global variables for timing parameters
uint32_t T0H_CYCLES, T0L_CYCLES, T1H_CYCLES, T1L_CYCLES, RESET_CYCLES;

// Global buffer for LED data (24 bits per LED)
uint32_t ws2812_buffer[NUM_LEDS];

// Function to calculate timing parameters based on clock frequency
void CalculateTimingParameters(uint32_t clock_freq) {
    float cycle_time = 1.0f / (float)clock_freq;  // Time per cycle in seconds
    cycle_time *= 1e6;                            // Convert to microseconds

    T0H_CYCLES = (uint32_t)(T0H_TIME / cycle_time);
    T0L_CYCLES = (uint32_t)(T0L_TIME / cycle_time);
    T1H_CYCLES = (uint32_t)(T1H_TIME / cycle_time);
    T1L_CYCLES = (uint32_t)(T1L_TIME / cycle_time);
    RESET_CYCLES = (uint32_t)(RESET_TIME / cycle_time);
}

// Function to initialize GPIO for WS2812
void WS2812_Init() {
    IO0DIR |= WS2812_PIN;  // Set P0.10 as output
    IO0CLR = WS2812_PIN;   // Set pin low initially
}

// Function to send a single bit to WS2812
void WS2812_SendBit(bool bit) {
    if (bit) {
        IO0SET = WS2812_PIN;  // Set pin high
        for (volatile int i = 0; i < T1H_CYCLES; i++);  // Wait for T1H
        IO0CLR = WS2812_PIN;  // Set pin low
        for (volatile int i = 0; i < T1L_CYCLES; i++);  // Wait for T1L
    } else {
        IO0SET = WS2812_PIN;  // Set pin high
        for (volatile int i = 0; i < T0H_CYCLES; i++);  // Wait for T0H
        IO0CLR = WS2812_PIN;  // Set pin low
        for (volatile int i = 0; i < T0L_CYCLES; i++);  // Wait for T0L
    }
}

// Function to send a single byte to WS2812
void WS2812_SendByte(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        WS2812_SendBit(byte & (1 << i));  // Send each bit
    }
}

// Function to send the entire LED data buffer
void WS2812_SendBuffer() {
    for (int i = 0; i < NUM_LEDS; i++) {
        // Send GRB data (24 bits per LED)
        WS2812_SendByte((ws2812_buffer[i] >> 16) & 0xFF);  // Green
        WS2812_SendByte((ws2812_buffer[i] >> 8) & 0xFF);   // Red
        WS2812_SendByte(ws2812_buffer[i] & 0xFF);          // Blue
    }

    // Send reset signal (low for >50탎)
    IO0CLR = WS2812_PIN;
    for (volatile int i = 0; i < RESET_CYCLES; i++);
}

// Function to set the color of an LED
void WS2812_SetColor(uint16_t index, uint8_t red, uint8_t green, uint8_t blue) {
    if (index < NUM_LEDS) {
        ws2812_buffer[index] = ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
    }
}

// Main function
int main() {
    // Set the clock frequency (in Hz)
    uint32_t clock_freq = 12000000;  // 12 MHz (change this if your clock is different)

    // Calculate timing parameters based on clock frequency
    CalculateTimingParameters(clock_freq);

    // Initialize WS2812
    WS2812_Init();

    // Set colors for LEDs
    WS2812_SetColor(0, 255, 0, 0);    // LED 0: Red
    WS2812_SetColor(1, 0, 255, 0);    // LED 1: Green
    WS2812_SetColor(2, 0, 0, 255);    // LED 2: Blue
    // Add more colors as needed...

    // Send the data to the WS2812 strip
    WS2812_SendBuffer();
    
    return 0;
}