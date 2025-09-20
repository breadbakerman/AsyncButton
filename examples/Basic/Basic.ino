/*
  AsyncButton Basic Example

  Demonstrates basic button press detection with different patterns:
  - Short press
  - Long press
  - Double press
  - Combined patterns

  This example uses the default BUTTON_OK pin configuration.
  Make sure to define BUTTON_OK in your config.h or modify the pin below.

  Hardware:
  - Connect a button between BUTTON_OK pin and ground
  - The library automatically enables INPUT_PULLUP

  Created by breadbaker, 2025
  This example code is in the public domain.
*/

#include <AsyncButton.h>

// If BUTTON_OK is not defined in config.h, define it here
#ifndef BUTTON_OK
#define BUTTON_OK 2 // Change this to your actual button pin
#endif

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // Wait for serial port to connect (needed for native USB)
    }

    Serial.println("AsyncButton Basic Example");
    Serial.println("========================");
    Serial.println("Press the button to see different patterns:");
    Serial.println("- Short press: Quick press and release");
    Serial.println("- Long press: Hold for 1+ seconds");
    Serial.println("- Double press: Press twice quickly");
    Serial.println();

    // Initialize AsyncButton with default configuration
    AsyncButton::setup();

    Serial.print("Button configured on pin ");
    Serial.println(BUTTON_OK);
    Serial.println("Ready! Press the button...");
    Serial.println();
}

void loop()
{
    // Must call update() regularly for non-blocking operation
    AsyncButton::update();

    // Check for short press
    if (AsyncButton::isShortPressed(BUTTON_OK, false))
    {
        Serial.println("✓ Short press detected");
    }

    // Check for long press
    if (AsyncButton::isLongPressed(BUTTON_OK, false))
    {
        Serial.println("⏰ Long press detected (1+ seconds)");
    }

    // Check for double press (any duration)
    if (AsyncButton::isPressedDouble(BUTTON_OK, false))
    {
        Serial.println("⚡ Double press detected");
    }

    // Check for short double press specifically
    if (AsyncButton::isShortPressedDouble(BUTTON_OK, false))
    {
        Serial.println("⚡✓ Short double press detected");
    }

    // Check for long double press specifically
    if (AsyncButton::isLongPressedDouble(BUTTON_OK, false))
    {
        Serial.println("⚡⏰ Long double press detected");
    }

    // Basic press detection (any press and release)
    if (AsyncButton::isPressed(BUTTON_OK, false))
    {
        Serial.println("📌 Basic press detected");
    }

    AsyncButton::reset(BUTTON_OK); // Manually reset state if needed

    // Your other code can go here
    // The button checking is non-blocking!
    delay(10); // Small delay for stability
}