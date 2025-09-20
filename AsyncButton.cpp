/* AsyncButton.cpp - Non-blocking button control library for Arduino
Copyright (c) 2025 by breadbaker
MIT License */
#include <AsyncButton.h>

#define ABUTTON_LOG_PREFIX ANSI_GRAY "[AsyncButton] " ANSI_DEFAULT

namespace AsyncButton
{
    static AsyncButton::State Buttons[] = {
        {BUTTON_OK, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},
        {BUTTON_CONFIRM, RELEASED, false, 0, 0, RELEASED, 0, 0, BUTTON_OK, nullptr},
        {BUTTON_CANCEL, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},
    };
    AsyncButton::Config ButtonConfig = {Buttons, sizeof(Buttons) / sizeof(AsyncButton::State)};

    AsyncButton::Config *Current = &ButtonConfig;
    static void (*buttonCallback)() = nullptr;

    void reset(const uint8_t pin)
    {
        State *button = getState(pin);
        if (!button)
            return;
        uint8_t savedMappedPin = button->mappedPin;
        State *savedMappedState = button->mappedState;
        *button = {button->pin, RELEASED, false, 0, 0, RELEASED, millis(), 0, savedMappedPin, savedMappedState};
        for (size_t i = 0; i < Current->size; ++i)
        {
            auto &mappedButton = Current->buttons[i];
            if (mappedButton.mappedPin == pin && &mappedButton != button)
            {
                uint8_t savedMappedPin2 = mappedButton.mappedPin;
                State *savedMappedState2 = mappedButton.mappedState;
                mappedButton = {mappedButton.pin, RELEASED, false, 0, 0, RELEASED, millis(), 0, savedMappedPin2, savedMappedState2};
            }
        }
    }

    void setup(void (*callback)(), uint8_t flags)
    {
        setup(ButtonConfig, callback, flags);
    }

    void setup(AsyncButton::Config &conf, void (*callback)(), uint8_t flags)
    {
        Current = &conf;
        buttonCallback = callback;
#ifndef BUTTON_SERIAL_DISABLE
        if (!(flags & BUT_SILENT))
        {
            SERIAL.print(F(ABUTTON_LOG_PREFIX "Setup long: " ANSI_YELLOW));
            SERIAL.print(BUTTON_LONGPRESS_TIME);
            SERIAL.print(F(ANSI_DEFAULT " double: " ANSI_YELLOW));
            SERIAL.print(BUTTON_DOUBLECLICK_TIME);
            SERIAL.print(F(ANSI_DEFAULT " debounce: " ANSI_YELLOW));
            SERIAL.print(BUTTON_DEBOUNCE_TIME);
            SERIAL.println(F(ANSI_DEFAULT "ms"));
        }
#endif
        for (size_t i = 0; i < Current->size; ++i)
            if (Current->buttons[i].pin != 255)
            {
                pinMode(Current->buttons[i].pin, INPUT_PULLUP);
                if (Current->buttons[i].mappedPin != 255)
                    Current->buttons[i].mappedState = getState(Current->buttons[i].mappedPin);
#ifndef BUTTON_SERIAL_DISABLE
                if (!(flags & BUT_SILENT))
                {
                    SERIAL.print(F(ABUTTON_LOG_PREFIX "Init button on pin: " ANSI_YELLOW));
                    SERIAL.print(Current->buttons[i].pin);
                    SERIAL.println(F(ANSI_DEFAULT));
                }
#endif
            }
    }

    void update()
    {
        unsigned long now = millis();
        for (size_t i = 0; i < Current->size; ++i)
        {
            auto &button = Current->buttons[i];
            if (button.pin == 255)
                continue;
            uint8_t reading = digitalRead(button.pin);
            if (reading != button.lastReading)
                button.lastChangeTime = now;
            if ((now - button.lastChangeTime) > BUTTON_DEBOUNCE_TIME)
            {
                if (reading != button.state)
                {
                    if (reading == PRESSED)
                    {
                        if (now - button.last_time < BUTTON_DOUBLECLICK_TIME)
                            button.doublePress = true;
                        else
                            button.doublePress = false;
                        button.duration = 0;
                        button.last_time = now;
                    }
                    else if (reading == RELEASED && button.state == PRESSED)
                        button.duration = now - button.last_time;
                    button.state = reading;
                }
            }
            button.lastReading = reading;
            if (reading == PRESSED &&
                button.state == PRESSED &&
                buttonCallback &&
                now - button.last_time >= BUTTON_LONGPRESS_TIME &&
                now - button.lastLongPressCallback >= BUTTON_LONGPRESS_TIME)
            {
                buttonCallback();
                button.lastLongPressCallback = now;
            }
            if (reading == RELEASED)
            {
                button.lastLongPressCallback = 0;
            }
        }
        for (size_t i = 0; i < Current->size; ++i)
        {
            auto &button = Current->buttons[i];
            if (button.mappedState && button.state == PRESSED)
            {
                button.mappedState->state = button.state;
                button.mappedState->duration = button.duration;
                button.mappedState->doublePress = button.doublePress;
                button.mappedState->last_time = button.last_time;
            }
        }
    }

    bool checkPress(const AsyncButton::State *button, unsigned long minDuration, unsigned long maxDuration, bool requireDouble, bool reset)
    {
        if (!button)
            return false;
        bool durationOk = button->duration > minDuration && (maxDuration == 0 || button->duration <= maxDuration);
        bool doubleOk = !requireDouble || button->doublePress;
        bool released = button->state == RELEASED && button->duration > 0 && millis() - button->last_time > BUTTON_DOUBLECLICK_TIME;
        if (released && durationOk && doubleOk)
        {
            if (reset)
                AsyncButton::reset(button->pin);
            return true;
        }
        return false;
    }

    bool isPressed(const uint8_t pin, bool reset)
    {
        return checkPress(getState(pin), 0, 0, false, reset);
    }

    bool isPressedDouble(const uint8_t pin, bool reset)
    {
        return checkPress(getState(pin), 0, 0, true, reset);
    }

    bool isShortPressed(const uint8_t pin, bool reset)
    {
        return checkPress(getState(pin), 0, BUTTON_LONGPRESS_TIME, false, reset);
    }

    bool isShortPressedDouble(const uint8_t pin, bool reset)
    {
        return checkPress(getState(pin), 0, BUTTON_LONGPRESS_TIME, true, reset);
    }

    bool isLongPressed(const uint8_t pin, bool reset)
    {
        return checkPress(getState(pin), BUTTON_LONGPRESS_TIME, 0, false, reset);
    }

    bool isLongPressedDouble(const uint8_t pin, bool reset)
    {
        return checkPress(getState(pin), BUTTON_LONGPRESS_TIME, 0, true, reset);
    }

    inline AsyncButton::State *getState(const uint8_t pin)
    {
        if (pin == 255)
            return nullptr;
        for (size_t i = 0; i < Current->size; ++i)
            if (Current->buttons[i].pin == pin)
                return &Current->buttons[i];
        return nullptr;
    }
}