<div align="right">
<img src="https://gist.githubusercontent.com/breadbakerman/70b7ee4430defddaf4f9323ffb73634c/raw/platformio-logo.svg" alt="" />
<img src="https://gist.githubusercontent.com/breadbakerman/8ca75dd6123b4d2882f26837436ef647/raw/arduino-logo.svg" alt="" />
</div>

# AsyncButton Library
[![Library Compile Test](https://github.com/breadbakerman/AsyncButton/actions/workflows/compile.yml/badge.svg)](https://github.com/breadbakerman/AsyncButton/actions/workflows/compile.yml)

A non-blocking Arduino library for button handling with support for multiple buttons, various press patterns, and advanced features like button mapping and customizable callbacks.

## Features

- **Non-blocking operation**: Uses `update()` calls instead of blocking delays for responsive UI
- **Multiple press patterns**: Short press, long press, double press, and their combinations
- **Multiple buttons**: Support for OK, CANCEL, CONFIRM buttons and custom pin configurations
- **Hardware debouncing**: Built-in debounce filtering eliminates mechanical bounce issues
- **Button mapping**: Map multiple physical buttons to the same logical button state
- **Callback support**: Optional callbacks for long press events with rate limiting
- **Configurable timing**: Customizable debounce, long press, and double-click detection windows

## Installation

1. Copy the `AsyncButton` folder to your Arduino libraries directory
2. Include the header in your sketch: `#include <AsyncButton.h>`

## Examples

The library includes example sketches to demonstrate usage:

- **Basic**: Simple button press detection with different patterns
- **Multiple**: Handling multiple buttons simultaneously
- **Mapping**: Button mapping and callback functionality

Access examples in the Arduino IDE via File → Examples → AsyncButton.

## Configuration

The library can be configured through compile-time definitions in an optional `config.h` or before including the header:

```cpp
#define BUTTON_SERIAL_DISABLE       // Disable serial output (set if SERIAL_OUT_DISABLE is defined)
#define BUTTON_OK 2                 // Pin for OK/Default button (255 = disabled)
#define BUTTON_CONFIRM 3            // Pin for Confirm button (255 = disabled)
#define BUTTON_CANCEL 4             // Pin for Cancel/Back button (255 = disabled)
#define BUTTON_DEBOUNCE_TIME 50     // Debounce time in milliseconds
#define BUTTON_DOUBLECLICK_TIME 400 // Double-click detection window in milliseconds
#define BUTTON_LONGPRESS_TIME 1000  // Long press threshold in milliseconds
```

### ANSI Color Configuration

The library supports optional ANSI color codes for serial output. If not defined in your `config.h`, they default to empty strings (no colors). To enable colored output, define these in your `config.h`:

```cpp
#define ANSI_GRAY "\e[38;5;8m"      // Gray color for log prefixes
#define ANSI_DEFAULT "\e[1;37m"     // Default white color
#define ANSI_YELLOW "\e[38;5;11m"   // Yellow color for values
```

The library automatically provides fallback empty definitions if these are not defined, ensuring compilation compatibility regardless of whether color support is configured.

## API Reference

The library provides various button press detection functions with optional reset behavior.

### Setup Functions

```cpp
// Initialize with default configuration and optional callback
void setup(void (*callback)() = nullptr, uint8_t flags = BUT_NONE);

// Initialize with custom configuration and optional callback
void setup(Config &conf, void (*callback)() = nullptr, uint8_t flags = BUT_NONE);
```

**Flags:**
- `BUT_NONE`: No special behavior
- `BUT_SHORT`: Short press mode
- `BUT_LONG`: Long press mode
- `BUT_DOUBLE`: Double press mode
- `BUT_SILENT`: Suppress console output

### Press Detection Functions

```cpp
// Basic press detection - any button press and release
bool isPressed(uint8_t pin, bool reset = true);

// Double press detection - two presses within double-click window
bool isPressedDouble(uint8_t pin, bool reset = true);

// Short press - press duration less than long press threshold
bool isShortPressed(uint8_t pin, bool reset = true);

// Short double press - double press with short press duration
bool isShortPressedDouble(uint8_t pin, bool reset = true);

// Long press - press duration greater than or equal to long press threshold
bool isLongPressed(uint8_t pin, bool reset = true);

// Long double press - double press with long press duration
bool isLongPressedDouble(uint8_t pin, bool reset = true);
```

### Utility Functions

```cpp
// Reset button state to prevent repeated triggers
void reset(uint8_t pin);
```

### Update Function

Non-blocking button detection requires regular `update()` calls to function properly.
```cpp
// Must be called regularly in main loop for non-blocking operation
void update();
```

## Configuration Structures

### State Structure
```cpp
struct State {
    uint8_t pin;                         // Pin number for the button
    uint8_t state;                       // Current state (PRESSED/RELEASED)
    bool doublePress;                    // Double press detection flag
    unsigned long duration;              // Press duration in milliseconds
    unsigned long last_time;             // Last state change timestamp
    uint8_t lastReading;                 // Last pin reading for debouncing
    unsigned long lastChangeTime;        // Last change time for debouncing
    unsigned long lastLongPressCallback; // Last long press callback timestamp
    uint8_t mappedPin;                   // Pin this button maps to (255 = no mapping)
    State *mappedState;                  // Pointer to mapped button's state
};
```

### Config Structure
```cpp
struct Config {
    State *buttons;  // Array of button states
    size_t size;     // Number of buttons in array
};
```

## Usage Examples

### Basic Setup and Usage

```cpp
#include <AsyncButton.h>

void setup() {
    Serial.begin(115200);
    // Initialize with default configuration (BUTTON_OK, BUTTON_CANCEL pins)
    AsyncButton::setup();
}

void loop() {
    // Must call update() for non-blocking operation
    AsyncButton::update();

    // Check for different press types
    if (AsyncButton::isShortPressed(BUTTON_OK)) {
        Serial.println("OK button short pressed");
    }

    if (AsyncButton::isLongPressed(BUTTON_OK)) {
        Serial.println("OK button long pressed");
    }

    if (AsyncButton::isShortPressedDouble(BUTTON_OK)) {
        Serial.println("OK button double-clicked");
    }
}
```

### Multiple Buttons

```cpp
#include <AsyncButton.h>

void setup() {
    Serial.begin(115200);
    AsyncButton::setup();
}

void loop() {
    AsyncButton::update();

    // Check OK button
    if (AsyncButton::isPressed(BUTTON_OK)) {
        Serial.println("OK pressed");
    }

    // Check CANCEL button
    if (AsyncButton::isPressed(BUTTON_CANCEL)) {
        Serial.println("CANCEL pressed");
    }

    // Check CONFIRM button
    if (AsyncButton::isPressed(BUTTON_CONFIRM)) {
        Serial.println("CONFIRM pressed");
    }
}
```

### Long Press Callback

```cpp
#include <AsyncButton.h>

void onLongPress() {
    Serial.println("Long press callback triggered!");
    // Handle global long press action here
}

void setup() {
    Serial.begin(115200);
    // Register callback for long press events
    AsyncButton::setup(onLongPress);
}

void loop() {
    AsyncButton::update();

    // Long press callback will be called automatically
    // Check for other press types as needed
    if (AsyncButton::isShortPressed(BUTTON_OK)) {
        Serial.println("Short press detected");
    }
}
```

### Custom Button Configuration

```cpp
#include <AsyncButton.h>

// Define custom button configuration for specific pins
AsyncButton::State myButtons[] = {
    {2, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},  // Button on pin 2
    {3, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},  // Button on pin 3
    {4, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},  // Button on pin 4
};
AsyncButton::Config myConfig = {myButtons, 3};

void setup() {
    Serial.begin(115200);
    AsyncButton::setup(myConfig);
}

void loop() {
    AsyncButton::update();

    if (AsyncButton::isPressed(2)) {
        Serial.println("Button 2 pressed");
    }

    if (AsyncButton::isPressed(3)) {
        Serial.println("Button 3 pressed");
    }

    if (AsyncButton::isPressed(4)) {
        Serial.println("Button 4 pressed");
    }
}
```

### Button Mapping

Button mapping allows multiple physical buttons to trigger the same logical button state, simplifying code while providing hardware flexibility.

```cpp
#include <AsyncButton.h>

// Custom button configuration with mapping
// BUTTON_CONFIRM maps to BUTTON_OK (pressing CONFIRM also triggers OK state)
AsyncButton::State myButtons[] = {
    {BUTTON_OK, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},           // OK button (no mapping)
    {BUTTON_CONFIRM, RELEASED, false, 0, 0, RELEASED, 0, 0, BUTTON_OK, nullptr}, // CONFIRM maps to OK
    {BUTTON_CANCEL, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},       // CANCEL button (no mapping)
};
AsyncButton::Config myConfig = {myButtons, 3};

void setup() {
    Serial.begin(115200);
    AsyncButton::setup(myConfig);
}

void loop() {
    AsyncButton::update();

    // Pressing physical CONFIRM button will trigger both CONFIRM and OK states
    if (AsyncButton::isPressed(BUTTON_CONFIRM)) {
        Serial.println("CONFIRM pressed");
    }

    if (AsyncButton::isPressed(BUTTON_OK)) {
        Serial.println("OK pressed (could be from OK or CONFIRM button)");
    }

    if (AsyncButton::isPressed(BUTTON_CANCEL)) {
        Serial.println("CANCEL pressed");
    }
}
```

### Advanced Press Pattern Detection

```cpp
#include <AsyncButton.h>

void setup() {
    Serial.begin(115200);
    AsyncButton::setup();
}

void loop() {
    AsyncButton::update();

    // Different press patterns for different actions
    if (AsyncButton::isShortPressed(BUTTON_OK)) {
        Serial.println("Quick action - OK short press");
    }

    if (AsyncButton::isLongPressed(BUTTON_OK)) {
        Serial.println("Power action - OK long press");
    }

    if (AsyncButton::isPressedDouble(BUTTON_OK)) {
        Serial.println("Special action - OK double press");
    }

    if (AsyncButton::isShortPressedDouble(BUTTON_OK)) {
        Serial.println("Quick special - OK short double press");
    }

    if (AsyncButton::isLongPressedDouble(BUTTON_OK)) {
        Serial.println("Power special - OK long double press");
    }
}
```

### State Management and Reset

```cpp
#include <AsyncButton.h>

bool menuMode = false;

void setup() {
    Serial.begin(115200);
    AsyncButton::setup();
}

void loop() {
    AsyncButton::update();

    if (AsyncButton::isLongPressed(BUTTON_OK)) {
        menuMode = !menuMode;
        Serial.println(menuMode ? "Entering menu mode" : "Exiting menu mode");

        // Reset button state to prevent immediate re-trigger
        AsyncButton::reset(BUTTON_OK);
    }

    if (menuMode) {
        // Menu navigation
        if (AsyncButton::isShortPressed(BUTTON_OK)) {
            Serial.println("Menu select");
        }

        if (AsyncButton::isShortPressed(BUTTON_CANCEL)) {
            Serial.println("Menu back");
        }
    } else {
        // Normal operation
        if (AsyncButton::isShortPressed(BUTTON_OK)) {
            Serial.println("Normal action");
        }
    }
}
```

## Press Pattern Detection

The library detects various button press patterns with configurable timing:

- **Short Press**: Press duration less than long press threshold (default: 1000ms)
- **Long Press**: Press duration greater than or equal to long press threshold
- **Double Press**: Two presses within double-click time window (default: 400ms)
- **Combined Patterns**: Short/long press combined with double-click detection

## Hardware Connection

Connect buttons between the specified pin and ground. The library automatically enables `INPUT_PULLUP` mode for all configured pins.

```
Button     Arduino
One end →  Pin (BUTTON_OK/BUTTON_CANCEL/etc.)
Other end → Ground
```

## Button Mapping

Button mapping allows multiple physical buttons to trigger the same logical button state. This feature provides:

- **Hardware flexibility**: Multiple physical buttons can activate the same function
- **Code simplification**: Handle one logical button state instead of multiple physical buttons
- **UI consistency**: Provide alternative input methods (e.g., CONFIRM and OK both trigger OK state)

### How Button Mapping Works:

1. **Configuration**: Set `mappedPin` to the target button's pin number during setup
2. **Initialization**: Library automatically resolves `mappedState` pointers during `setup()`
3. **State Propagation**: When a mapped button is pressed, its state is copied to the target button
4. **Reset Behavior**: Resetting a target button also resets all buttons that map to it

## Dependencies

- Arduino core libraries
- `config.h` (optional, auto-detected for project-specific settings)

## Notes

- Always call `AsyncButton::update()` in your main loop for proper non-blocking operation
- Button states use hardware debouncing with configurable timing (default: 50ms)
- Long press callbacks are rate-limited to prevent excessive triggering
- Use `reset = true` (default parameter) to prevent repeated triggers from the same press
- Set pin to 255 to disable unused buttons in default configuration
- All timing operations use `millis()` for non-blocking behavior
- The library automatically handles pin mode configuration (`INPUT_PULLUP`)

## License

MIT License - Copyright (c) 2025 by breadbaker
