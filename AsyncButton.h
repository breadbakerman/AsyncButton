/* AsyncButton.h - Non-blocking button control library for Arduino
Copyright (c) 2025 by breadbaker
MIT License */
#pragma once
#include <Arduino.h>
#if __has_include(<config.h>)
#include <config.h> // Include a global config if available
#endif

#ifdef SERIAL_OUT_DISABLE
#define BUTTON_SERIAL_DISABLE // Disable Serial output
#endif

#if !defined(BUTTON_SERIAL_DISABLE)
#undef SERIAL // Serial port mapping
#ifdef SERIAL_PORT_USBVIRTUAL
#define SERIAL SERIAL_PORT_USBVIRTUAL
#else
#define SERIAL Serial // Use default Serial port
#endif
#endif

#ifndef BUTTON_OK
#define BUTTON_OK 255 // Pin for OK/Default button
#endif
#ifndef BUTTON_CONFIRM
#define BUTTON_CONFIRM 255 // Pin for Confirm button
#endif
#ifndef BUTTON_CANCEL
#define BUTTON_CANCEL 255 // Pin for Cancel/Back button
#endif
#ifndef BUTTON_DEBOUNCE_TIME
#define BUTTON_DEBOUNCE_TIME 50 // Debounce time in milliseconds
#endif
#ifndef BUTTON_DOUBLECLICK_TIME
#define BUTTON_DOUBLECLICK_TIME 400 // Time to consider a double click in milliseconds
#endif
#ifndef BUTTON_LONGPRESS_TIME
#define BUTTON_LONGPRESS_TIME 1000 // Time to consider a long press in milliseconds
#endif

#define PRESSED LOW   // Button pressed state
#define RELEASED HIGH // Button released state

// Flags:
#define BUT_NONE 0x00
#define BUT_SHORT 0x01
#define BUT_LONG 0x02
#define BUT_DOUBLE 0x04
#define BUT_SILENT 0x80

// Define ANSI color codes if not already defined:
#ifndef ANSI_GRAY
#define ANSI_GRAY ""
#endif
#ifndef ANSI_DEFAULT
#define ANSI_DEFAULT ""
#endif
#ifndef ANSI_YELLOW
#define ANSI_YELLOW ""
#endif

namespace AsyncButton
{
    struct State
    {
        uint8_t pin;                         // Pin number for the button
        uint8_t state;                       // Current state of the button (PRESSED/RELEASED)
        bool doublePress;                    // Flag for double press detection
        unsigned long duration;              // Duration the button has been pressed
        unsigned long last_time;             // Last time the button state was changed
        uint8_t lastReading;                 // Last reading of the button state
        unsigned long lastChangeTime;        // Last time the button state changed
        unsigned long lastLongPressCallback; // Last time long press callback was called
        uint8_t mappedPin;                   // Pin of the button this maps to (255 = no mapping)
        State *mappedState;                  // Direct pointer to the mapped button's State
    };

    struct Config
    {
        State *buttons; // Array of button states
        size_t size;    // Size of the array
    };

    extern AsyncButton::Config ButtonConfig;
    extern AsyncButton::Config *Current;
    void reset(const uint8_t pin);
    void setup(void (*callback)() = nullptr, uint8_t flags = BUT_NONE);
    void setup(AsyncButton::Config &conf, void (*callback)() = nullptr, uint8_t flags = BUT_NONE);
    void update();
    bool isPressed(const uint8_t pin, bool reset = true);
    bool isPressedDouble(const uint8_t pin, bool reset = true);
    bool isShortPressed(const uint8_t pin, bool reset = true);
    bool isShortPressedDouble(const uint8_t pin, bool reset = true);
    bool isLongPressed(const uint8_t pin, bool reset = true);
    bool isLongPressedDouble(const uint8_t pin, bool reset = true);
    inline AsyncButton::State *getState(const uint8_t pin);
}