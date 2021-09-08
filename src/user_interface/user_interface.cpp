#include "user_interface.h"
#include "fonts/roboto_bold_8.h"
#include "fonts/roboto_bold_10.h"
#include "../pins/pins.h"
#include "../firebase/firebase.h"
#include "../unix_time/unix_time.h"

UIPage currentUIPage = UIPage::P_HOME;
HomeOption currentHomeOption = HomeOption::HO_ENABLE_LOGGING;
UserAction currentUserAction = UserAction::UA_NONE;

unsigned long logIntervalTemp = 1000;    // Milliseconds // Store temp value
float temperatureLimitTemp = 25; // Celcius // Store temp value

void userInterfaceSetup()
{
    oledDisplay.init();
    oledDisplay.clear();
    oledDisplay.drawString(0, 0, "Wait ah...");
    oledDisplay.display();
}

void processInputs()
{
    int pressEnter = digitalRead(button_enter);
    int pressBack = digitalRead(button_back);
    int pressLeft = digitalRead(button_left);
    int pressRight = digitalRead(button_right);
    if (pressEnter == LOW)
    {
        currentUserAction = UserAction::UA_ENTER;
        Serial.println("User Action: ENTER");
    }
    else if (pressBack == LOW)
    {
        currentUserAction = UserAction::UA_BACK;
        Serial.println("User Action: BACK");
    }
    else if (pressLeft == LOW)
    {
        currentUserAction = UserAction::UA_LEFT;
        Serial.println("User Action: LEFT");
    }
    else if (pressRight == LOW)
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
                updateConfigFirebase();
            }
            else if (currentHomeOption == HomeOption::HO_EDIT_LOG_INTERVAL)
            {
                currentUIPage = UIPage::P_EDIT_LOG_INTERVAL;
                logIntervalTemp = logInterval;
                displayEditLogInterval();
            }
            else if (currentHomeOption == HomeOption::HO_EDIT_TEMPERATURE_LIMIT)
            {
                currentUIPage = UIPage::P_EDIT_TEMPERATURE_LIMIT;
                temperatureLimitTemp = temperatureLimit;
                displayEditTemperatureLimit();
            }
        }
        else
        {
            const char index= static_cast<char>(currentHomeOption);
            if (currentUserAction == UserAction::UA_LEFT)
            {
                if (index == 0) {
                    currentHomeOption = HomeOption::HO_EDIT_TEMPERATURE_LIMIT;
                }
                else {
                    currentHomeOption = static_cast<HomeOption>(index - 1);
                }
            }
            else if (currentUserAction == UserAction::UA_RIGHT)
            {
                currentHomeOption = static_cast<HomeOption>((index + 1) % 3);
            }
            displayHome();
        }
    }
    else if (currentUIPage == UIPage::P_EDIT_LOG_INTERVAL)
    {
        if (currentUserAction == UserAction::UA_BACK)
        {
            currentUIPage = UIPage::P_HOME;
            displayHome();
        }
        else
        {
            if (currentUserAction == UserAction::UA_LEFT)
            {
                logIntervalTemp = max(logIntervalTemp - logIntervalStep, (unsigned long) minLogInterval);
            }
            else if (currentUserAction == UserAction::UA_RIGHT)
            {
                logIntervalTemp += logIntervalStep;
            }
            else if (currentUserAction == UserAction::UA_ENTER)
            {
                logInterval = logIntervalTemp;
                updateConfigFirebase();
            }
            displayEditLogInterval();
        }
    }
    else if (currentUIPage == UIPage::P_EDIT_TEMPERATURE_LIMIT)
    {
        if (currentUserAction == UserAction::UA_BACK)
        {
            currentUIPage = UIPage::P_HOME;
            displayHome();
        }
        else
        {
            if (currentUserAction == UserAction::UA_LEFT)
            {
                temperatureLimitTemp = max(temperatureLimitTemp - temperatureLimitStep, minTemperatureLimit);
            }
            else if (currentUserAction == UserAction::UA_RIGHT)
            {
                temperatureLimitTemp += temperatureLimitStep;
            }
            else if (currentUserAction == UserAction::UA_ENTER)
            {
                temperatureLimit = temperatureLimitTemp;
                updateConfigFirebase();
            }
            displayEditTemperatureLimit();
        }
    }
    currentUserAction = UA_NONE;
}
// 128 x 64, width x height
void displayHome()
{
    // 3 SELECTABLE options at the top
    // logInterval, temperatureValue and temperaturelimit at the middle
    // date, time and wifi connectivity at the bottom
    oledDisplay.clear();

/*     oledDisplay.drawString(0, 24,"Logging Interval: ");
    oledDisplay.drawString(70, 24, String(temperatureLimit));  
    oledDisplay.drawString(110, 24,"*C"); // ºC */

    oledDisplay.setFont(Roboto_Bold_8);
    oledDisplay.setTextAlignment(TEXT_ALIGN_LEFT);
    oledDisplay.drawStringMaxWidth(0, 0, 40, enableLogging?"Disable Log":"Enable Log");
    if (currentHomeOption == HomeOption::HO_ENABLE_LOGGING) {
        oledDisplay.drawRect(0, 0, 30, 22);
    }
    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawStringMaxWidth(64, 0, 40, "Log Interval");
    if (currentHomeOption == HomeOption::HO_EDIT_LOG_INTERVAL) {
        oledDisplay.drawRect(50, 0, 30, 22);
    }
    oledDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
    oledDisplay.drawStringMaxWidth(128, 0, 40, "Temp. Limit");
    if (currentHomeOption == HomeOption::HO_EDIT_TEMPERATURE_LIMIT) {
        oledDisplay.drawRect(98, 0, 30, 22);
    }

    oledDisplay.setFont(Roboto_Bold_10);
    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(32, 28, String(temperatureValue) + "*C");

    if (temperatureValue > temperatureLimit)
    {
        oledDisplay.setFont(Roboto_Bold_8);
        oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
        oledDisplay.drawString(32, 44, "Exceeding limit!");
    }

/*     oledDisplay.setFont(Roboto_Bold_8);
    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(32, 44, "Limit:" + String(temperatureLimit) + "*C"); */

/*     oledDisplay.setFont(Roboto_Bold_8);
    oledDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
    oledDisplay.drawString(128, 28, "Log:" String(logInterval/1000.0) + "secs"); */

    oledDisplay.setFont(Roboto_Bold_8);
    oledDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
    oledDisplay.drawString(128, 28, WiFi.status() == WL_CONNECTED ? "Online" : "Offline");

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
    // 2 static options at the top
    // previous logInterval and new logInterval at the middle
    // date, time and wifi connectivity at the bottom
    oledDisplay.clear();
    oledDisplay.setFont(Roboto_Bold_10);

    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(64, 5, "Log Interval");
    oledDisplay.display();

    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(64, 30, String(logIntervalTemp / 1000.0));
    oledDisplay.display();
}

void displayEditTemperatureLimit()
{
    // 2 static options at the top
    // previous temperatureLimit and new temperatureLimit at the middle
    // date, time and wifi connectivity at the bottom
    oledDisplay.clear();
    oledDisplay.setFont(Roboto_Bold_10);

    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(64, 5, "Temperature Limit");
    oledDisplay.display();

    oledDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    oledDisplay.drawString(64, 30, String(temperatureLimitTemp));
    oledDisplay.display();
}