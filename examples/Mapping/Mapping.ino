/*
  AsyncButton Mapping and Callback Example

  Demonstrates advanced features:
  - Button mapping (multiple physical buttons → same logical state)
  - Long press callbacks with rate limiting
  - Custom button configuration
  - State management and reset behavior

  This example shows how CONFIRM button can be mapped to trigger
  the same state as OK button, simplifying code while providing
  hardware flexibility.

  Hardware:
  - Connect buttons between each pin and ground
  - The library automatically enables INPUT_PULLUP

  Created by breadbaker, 2025
  This example code is in the public domain.
*/

#include <AsyncButton.h>

// Define button pins - modify these for your hardware
#define BUTTON_OK_PIN 2
#define BUTTON_CONFIRM_PIN 3
#define BUTTON_CANCEL_PIN 4

// Global state for demo
int confirmCount = 0;
int cancelCount = 0;
bool longPressActive = false;

// Long press callback function
void onLongPress()
{
    if (!longPressActive)
    {
        longPressActive = true;
        Serial.println();
        Serial.println("🔔 LONG PRESS CALLBACK TRIGGERED!");
        Serial.println("   This callback is rate-limited to prevent spam");
        Serial.println("   Release and press again to trigger another callback");
        Serial.println();
    }
}

// Custom button configuration with mapping
// CONFIRM button maps to OK button (pressing CONFIRM also triggers OK state)
AsyncButton::State buttons[] = {
    // Pin, State, DoublePress, Duration, LastTime, LastReading, LastChangeTime, LastLongPressCallback, MappedPin, MappedState
    {BUTTON_OK_PIN, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},                // OK button (no mapping)
    {BUTTON_CONFIRM_PIN, RELEASED, false, 0, 0, RELEASED, 0, 0, BUTTON_OK_PIN, nullptr}, // CONFIRM maps to OK
    {BUTTON_CANCEL_PIN, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},            // CANCEL button (no mapping)
};

AsyncButton::Config buttonConfig = {buttons, 3};

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // Wait for serial port to connect (needed for native USB)
    }

    Serial.println("AsyncButton Mapping and Callback Example");
    Serial.println("========================================");
    Serial.println("Hardware setup:");
    Serial.print("OK button on pin ");
    Serial.println(BUTTON_OK_PIN);
    Serial.print("CONFIRM button on pin ");
    Serial.print(BUTTON_CONFIRM_PIN);
    Serial.println(" (maps to OK)");
    Serial.print("CANCEL button on pin ");
    Serial.println(BUTTON_CANCEL_PIN);
    Serial.println();

    // Initialize AsyncButton with custom configuration and callback
    AsyncButton::setup(buttonConfig, onLongPress);

    Serial.println("Button Mapping Demo:");
    Serial.println("- Pressing OK button → triggers OK state only");
    Serial.println("- Pressing CONFIRM button → triggers BOTH CONFIRM and OK states");
    Serial.println("- Pressing CANCEL button → triggers CANCEL state only");
    Serial.println();
    Serial.println("Long Press Callback:");
    Serial.println("- Hold any button for 1+ seconds to trigger callback");
    Serial.println("- Callback is rate-limited to prevent spam");
    Serial.println();
    Serial.println("Try it out!");
    Serial.println();
}

void loop()
{
    // Must call update() regularly for non-blocking operation
    AsyncButton::update();

    // Reset long press flag when no buttons are being held
    if (!AsyncButton::isLongPressed(BUTTON_OK_PIN, false) &&
        !AsyncButton::isLongPressed(BUTTON_CONFIRM_PIN, false) &&
        !AsyncButton::isLongPressed(BUTTON_CANCEL_PIN, false))
    {
        longPressActive = false;
    }

    // Handle OK button (can be triggered by OK or CONFIRM physical buttons)
    if (AsyncButton::isShortPressed(BUTTON_OK_PIN))
    {
        confirmCount++;
        Serial.print("✅ OK/CONFIRM action #");
        Serial.print(confirmCount);
        Serial.println(" (could be from OK or CONFIRM button)");

        if (confirmCount % 5 == 0)
        {
            Serial.println("🎉 Every 5th confirmation! Special action triggered!");
        }
    }

    // Handle CONFIRM button specifically (only triggered by CONFIRM physical button)
    if (AsyncButton::isShortPressed(BUTTON_CONFIRM_PIN))
    {
        Serial.println("📝 CONFIRM button specifically pressed");
        Serial.println("   (This message appears in addition to OK action above)");
    }

    // Handle CANCEL button
    if (AsyncButton::isShortPressed(BUTTON_CANCEL_PIN))
    {
        cancelCount++;
        Serial.print("❌ CANCEL action #");
        Serial.println(cancelCount);

        if (cancelCount >= 3)
        {
            Serial.println("🔄 Resetting counters after 3 cancellations...");
            confirmCount = 0;
            cancelCount = 0;

            // Demonstrate reset functionality
            AsyncButton::reset(BUTTON_CANCEL_PIN);
            Serial.println("   CANCEL button state reset");
        }
    }

    // Handle double presses
    if (AsyncButton::isPressedDouble(BUTTON_OK_PIN))
    {
        Serial.println("⚡ Double press detected on OK (could be OK or CONFIRM button)");
    }

    if (AsyncButton::isPressedDouble(BUTTON_CANCEL_PIN))
    {
        Serial.println("⚡❌ Double press on CANCEL - Emergency reset!");
        confirmCount = 0;
        cancelCount = 0;
        longPressActive = false;

        // Reset all button states
        AsyncButton::reset(BUTTON_OK_PIN);
        AsyncButton::reset(BUTTON_CONFIRM_PIN);
        AsyncButton::reset(BUTTON_CANCEL_PIN);

        Serial.println("🔄 All counters and button states reset!");
    }

    // Demonstrate different press patterns
    if (AsyncButton::isLongPressed(BUTTON_OK_PIN, false))
    { // Don't auto-reset
        if (!longPressActive)
        {
            Serial.println("⏰ Long press in progress on OK button...");
        }
    }

    if (AsyncButton::isLongPressed(BUTTON_CANCEL_PIN, false))
    { // Don't auto-reset
        if (!longPressActive)
        {
            Serial.println("⏰❌ Long press in progress on CANCEL - hold to trigger callback...");
        }
    }

    // Show status every so often
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 10000)
    { // Every 10 seconds
        lastStatus = millis();
        showStatus();
    }

    delay(10); // Small delay for stability
}

void showStatus()
{
    Serial.println();
    Serial.println("📊 STATUS REPORT:");
    Serial.print("   Confirmations: ");
    Serial.println(confirmCount);
    Serial.print("   Cancellations: ");
    Serial.println(cancelCount);
    Serial.println("   Button mapping is working correctly!");
    Serial.println("   Long press callback is active");
    Serial.println();
}

// Additional utility functions to demonstrate advanced usage

void demonstrateMapping()
{
    Serial.println("🔍 MAPPING DEMONSTRATION:");
    Serial.println("Watch what happens when you press each button:");
    Serial.println();

    Serial.println("1. Press OK button:");
    Serial.println("   → Only OK state will be triggered");
    Serial.println();

    Serial.println("2. Press CONFIRM button:");
    Serial.println("   → Both CONFIRM and OK states will be triggered");
    Serial.println("   → This is because CONFIRM maps to OK");
    Serial.println();

    Serial.println("3. Press CANCEL button:");
    Serial.println("   → Only CANCEL state will be triggered");
    Serial.println();
}