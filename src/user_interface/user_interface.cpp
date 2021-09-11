#include "user_interface.h"
#include "fonts/roboto_bold_8.h"
#include "fonts/roboto_bold_10.h"
#include "../pins/pins.h"
#include "../firebase/firebase.h"
#include "../unix_time/unix_time.h"

UserAction currentUserAction = UserAction::UA_NONE; // Current user action state
UIPage currentUIPage = UIPage::P_HOME; // Current UI page state
HomeOption currentHomeOption = HomeOption::HO_ENABLE_LOGGING; // Current home option state

unsigned long logIntervalTemp = 1000; // Milliseconds // Store temporary value
float temperatureLimitTemp = 25; // Celcius // Store temporary value

void userInterfaceSetup()
{
    oledDisplay.init(); // Initialize OLED
    oledDisplay.clear();
    oledDisplay.drawString(0, 0, "Wait ah...");
    oledDisplay.display();
}

void processInputs()
{
    // Checks each button and sets the current user action
    if (digitalRead(button_enter) == LOW)
    {
        currentUserAction = UserAction::UA_ENTER;
        Serial.println("User Action: ENTER");
    }
    else if (digitalRead(button_back) == LOW)
    {
        currentUserAction = UserAction::UA_BACK;
        Serial.println("User Action: BACK");
    }
    else if (digitalRead(button_left) == LOW)
    {
        currentUserAction = UserAction::UA_LEFT;
        Serial.println("User Action: LEFT");
    }
    else if (digitalRead(button_right) == LOW)
    {
        currentUserAction = UserAction::UA_RIGHT;
        Serial.println("User Action: RIGHT");
    }
}

void renderUI()
{
    if (currentUIPage == UIPage::P_HOME)
    {
        if (currentUserAction == UserAction::UA_ENTER)
        {
            if (currentHomeOption == HomeOption::HO_ENABLE_LOGGING)
            {
                enableLogging = !enableLogging;
                updateConfigFirebase(); // Update Firebase
            }
            else if (currentHomeOption == HomeOption::HO_EDIT_LOG_INTERVAL)
            {
                currentUIPage = UIPage::P_EDIT_LOG_INTERVAL; // Change page
                logIntervalTemp = logInterval; // Initial temporary value
                displayEditLogInterval(); // Display on OLED
            }
            else if (currentHomeOption == HomeOption::HO_EDIT_TEMPERATURE_LIMIT)
            {
                currentUIPage = UIPage::P_EDIT_TEMPERATURE_LIMIT; // Change page
                temperatureLimitTemp = temperatureLimit; // Initial temporary value
                displayEditTemperatureLimit(); // Display on OLED
            }
        }
        else
        {
            const char index= static_cast<char>(currentHomeOption); // Current Home option index
            if (currentUserAction == UserAction::UA_LEFT)
            {
                if (index == 0) {
                    currentHomeOption = HomeOption::HO_EDIT_TEMPERATURE_LIMIT; // Change Home option
                }
                else {
                    currentHomeOption = static_cast<HomeOption>(index - 1); // Change to left Home option
                }
            }
            else if (currentUserAction == UserAction::UA_RIGHT)
            {
                currentHomeOption = static_cast<HomeOption>((index + 1) % 3); // Change to right Home option.
                // Modulo 3 because there's 3 options
            }
            displayHome(); // Display on OLED
        }
    }
    else if (currentUIPage == UIPage::P_EDIT_LOG_INTERVAL)
    {
        if (currentUserAction == UserAction::UA_BACK)
        {
            currentUIPage = UIPage::P_HOME; // Cancel and go back to Home page
            displayHome(); // Display on OLED
        }
        else
        {
            if (currentUserAction == UserAction::UA_LEFT)
            {
                // Decrement by step but ensure it is not lower than minimum
                logIntervalTemp = max(logIntervalTemp - logIntervalStep, (unsigned long) minLogInterval);
            }
            else if (currentUserAction == UserAction::UA_RIGHT)
            {
                logIntervalTemp += logIntervalStep; // Increment by step
            }
            else if (currentUserAction == UserAction::UA_ENTER)
            {
                logInterval = logIntervalTemp; // Update to new value
                updateConfigFirebase(); // Update Firebase
            }
            displayEditLogInterval(); // Display on OLED
        }
    }
    else if (currentUIPage == UIPage::P_EDIT_TEMPERATURE_LIMIT)
    {
        if (currentUserAction == UserAction::UA_BACK)
        {
            currentUIPage = UIPage::P_HOME; // Cancel and go back to Home page
            displayHome(); // Display on OLED
        }
        else
        {
            if (currentUserAction == UserAction::UA_LEFT)
            {
                // Decrement by step but ensure it is not lower than minimum
                temperatureLimitTemp = max(temperatureLimitTemp - temperatureLimitStep, minTemperatureLimit);
            }
            else if (currentUserAction == UserAction::UA_RIGHT)
            {
                temperatureLimitTemp += temperatureLimitStep; // Increment by step
            }
            else if (currentUserAction == UserAction::UA_ENTER)
            {
                temperatureLimit = temperatureLimitTemp; // Update to new value
                updateConfigFirebase(); // Update Firebase
            }
            displayEditTemperatureLimit(); // Display on OLED
        }
    }
    currentUserAction = UA_NONE; // Reset user action
}

// (128 x 64), (width x height)
void displayHome()
{
    // 3 selectable options at the top
    // temperatureValue and warning at the middle-left
    // Wi-Fi connectivity at the middle-right
    // Current time at the bottom-right
    oledDisplay.clear();
    oledDisplay.setFont(Roboto_Bold_8);
    // enableLogging option
    oledDisplay.setTextAlignment(TEXT_ALIGN_LEFT);
    oledDisplay.drawStringMaxWidth(0, 0, 40, enableLogging?"Disable Log":"Enable Log");
    if (currentHomeOption == HomeOption::HO_ENABLE_LOGGING) {
        oledDisplay.drawRect(0, 0, 30, 22);
    }
    // Edit logInterval option
    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawStringMaxWidth(64, 0, 40, "Log Interval");
    if (currentHomeOption == HomeOption::HO_EDIT_LOG_INTERVAL) {
        oledDisplay.drawRect(50, 0, 30, 22);
    }
    // Edit temperatureLimit option
    oledDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
    oledDisplay.drawStringMaxWidth(128, 0, 40, "Temp. Limit");
    if (currentHomeOption == HomeOption::HO_EDIT_TEMPERATURE_LIMIT) {
        oledDisplay.drawRect(98, 0, 30, 22);
    }
    // Display temperatureValue
    oledDisplay.setFont(Roboto_Bold_10);
    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(32, 28, String(temperatureValue) + "*C");

    if (temperatureValue > temperatureLimit)
    {
        // Display warning if exceed temperatureLimit
        oledDisplay.setFont(Roboto_Bold_8);
        oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
        oledDisplay.drawString(32, 44, "Exceeding limit!");
    }

    // Display Wi-Fi connectivity
    oledDisplay.setFont(Roboto_Bold_8);
    oledDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
    oledDisplay.drawString(128, 28, WiFi.status() == WL_CONNECTED ? "Online" : "Offline");

    // Display current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
    }
    else {
        char timeString[10];
        strftime(timeString, 10, "%H:%M:%S", &timeinfo);
        oledDisplay.setFont(Roboto_Bold_8);
        oledDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
        oledDisplay.drawString(128, 50, timeString);
        oledDisplay.display();
    }
}

void displayEditLogInterval()
{
    oledDisplay.clear();
    oledDisplay.setFont(Roboto_Bold_10);
    // Display title
    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(64, 5, "Log Interval");
    oledDisplay.display();
    // Display temporary logInterval
    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(64, 30, String(logIntervalTemp / 1000.0));
    oledDisplay.display();
}

void displayEditTemperatureLimit()
{
    oledDisplay.clear();
    oledDisplay.setFont(Roboto_Bold_10);
    // Display title
    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(64, 5, "Temperature Limit");
    oledDisplay.display();
    // Display temporary temperatureLimit
    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(64, 30, String(temperatureLimitTemp));
    oledDisplay.display();
}