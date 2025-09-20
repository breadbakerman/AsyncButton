/*
  AsyncButton Multiple Buttons Example

  Demonstrates handling multiple buttons simultaneously:
  - OK button for confirmations
  - CANCEL button for cancellations
  - CONFIRM button for confirmations
  - Custom pin configuration

  This example shows how to work with multiple buttons and
  provides a simple menu-like interface.

  Hardware:
  - Connect buttons between each pin and ground
  - The library automatically enables INPUT_PULLUP

  Created by breadbaker, 2025
  This example code is in the public domain.
*/

#include <AsyncButton.h>

// Define button pins - modify these for your hardware
#define BUTTON_OK_PIN 2
#define BUTTON_CANCEL_PIN 3
#define BUTTON_CONFIRM_PIN 4

// Custom button configuration
AsyncButton::State buttons[] = {
    {BUTTON_OK_PIN, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},      // OK button
    {BUTTON_CANCEL_PIN, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr},  // CANCEL button
    {BUTTON_CONFIRM_PIN, RELEASED, false, 0, 0, RELEASED, 0, 0, 255, nullptr}, // CONFIRM button
};

AsyncButton::Config buttonConfig = {buttons, 3};

// Simple menu state
int menuItem = 0;
const char *menuItems[] = {"Start Process", "Settings", "About", "Exit"};
const int menuCount = 4;
bool inMenu = true;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // Wait for serial port to connect (needed for native USB)
    }

    Serial.println("AsyncButton Multiple Buttons Example");
    Serial.println("====================================");
    Serial.println("Hardware setup:");
    Serial.print("OK button on pin ");
    Serial.println(BUTTON_OK_PIN);
    Serial.print("CANCEL button on pin ");
    Serial.println(BUTTON_CANCEL_PIN);
    Serial.print("CONFIRM button on pin ");
    Serial.println(BUTTON_CONFIRM_PIN);
    Serial.println();

    // Initialize AsyncButton with custom configuration
    AsyncButton::setup(buttonConfig);

    Serial.println("Controls:");
    Serial.println("- OK: Navigate down / Select");
    Serial.println("- CANCEL: Navigate up / Back");
    Serial.println("- CONFIRM: Confirm selection");
    Serial.println("- Long press any button: Show button info");
    Serial.println();

    showMenu();
}

void loop()
{
    // Must call update() regularly for non-blocking operation
    AsyncButton::update();

    if (inMenu)
    {
        handleMenuInput();
    }

    // Handle long presses for any button (info display)
    if (AsyncButton::isLongPressed(BUTTON_OK_PIN))
    {
        Serial.println("ℹ️  OK button - Used for navigation and selection");
    }

    if (AsyncButton::isLongPressed(BUTTON_CANCEL_PIN))
    {
        Serial.println("ℹ️  CANCEL button - Used for navigation and going back");
    }

    if (AsyncButton::isLongPressed(BUTTON_CONFIRM_PIN))
    {
        Serial.println("ℹ️  CONFIRM button - Used for confirming actions");
    }

    // Your other application code can go here
    delay(10); // Small delay for stability
}

void handleMenuInput()
{
    // OK button: Navigate down or select
    if (AsyncButton::isShortPressed(BUTTON_OK_PIN))
    {
        menuItem = (menuItem + 1) % menuCount;
        showMenu();
    }

    // CANCEL button: Navigate up
    if (AsyncButton::isShortPressed(BUTTON_CANCEL_PIN))
    {
        menuItem = (menuItem - 1 + menuCount) % menuCount;
        showMenu();
    }

    // CONFIRM button: Select current item
    if (AsyncButton::isShortPressed(BUTTON_CONFIRM_PIN))
    {
        selectMenuItem();
    }

    // Double press OK: Quick select
    if (AsyncButton::isShortPressedDouble(BUTTON_OK_PIN))
    {
        Serial.println("⚡ Quick select!");
        selectMenuItem();
    }
}

void showMenu()
{
    Serial.println("=== MENU ===");
    for (int i = 0; i < menuCount; i++)
    {
        if (i == menuItem)
        {
            Serial.print(" > "); // Current selection
        }
        else
        {
            Serial.print("   ");
        }
        Serial.print(i + 1);
        Serial.print(". ");
        Serial.println(menuItems[i]);
    }
    Serial.println("============");
}

void selectMenuItem()
{
    Serial.print("Selected: ");
    Serial.println(menuItems[menuItem]);

    switch (menuItem)
    {
    case 0: // Start Process
        Serial.println("🚀 Starting process...");
        simulateProcess();
        break;

    case 1: // Settings
        Serial.println("⚙️  Opening settings...");
        showSettings();
        break;

    case 2: // About
        Serial.println("📋 About this example:");
        Serial.println("   AsyncButton Multiple Buttons Demo");
        Serial.println("   Demonstrates multi-button handling");
        break;

    case 3: // Exit
        Serial.println("👋 Goodbye!");
        inMenu = false;
        break;
    }

    if (inMenu)
    {
        Serial.println("Press any button to continue...");
        waitForAnyButton();
        showMenu();
    }
}

void simulateProcess()
{
    Serial.println("Process running... (Press CANCEL to abort)");

    for (int i = 0; i < 10; i++)
    {
        AsyncButton::update();

        if (AsyncButton::isPressed(BUTTON_CANCEL_PIN))
        {
            Serial.println("❌ Process aborted by user");
            return;
        }

        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.println("✅ Process completed successfully!");
}

void showSettings()
{
    Serial.println("Settings Menu:");
    Serial.println("1. Timing: Debounce=50ms, LongPress=1000ms, DoubleClick=400ms");
    Serial.println("2. All buttons use INPUT_PULLUP mode");
    Serial.println("3. Non-blocking operation enabled");
}

void waitForAnyButton()
{
    while (true)
    {
        AsyncButton::update();

        if (AsyncButton::isPressed(BUTTON_OK_PIN) ||
            AsyncButton::isPressed(BUTTON_CANCEL_PIN) ||
            AsyncButton::isPressed(BUTTON_CONFIRM_PIN))
        {
            break;
        }

        delay(10);
    }
}