#include "user_interface.h"
#include "../pins/pins.h"
#include "../firebase/firebase.h"

UIPage currentUIPage = UIPage::P_HOME;
HomeOption currentHomeOption = HomeOption::HO_EDIT_LOG_INTERVAL;
UserAction currentUserAction = UserAction::UA_NONE;

float logIntervalTemp = 1000;    // Milliseconds // Store temp value
float temperatureLimitTemp = 25; // Celcius // Store temp value

void userInterfaceSetup()
{
    lcdDisplay.init();
    lcdDisplay.clear();
    lcdDisplay.drawString(0, 0, "Wait ah...");
    lcdDisplay.display();
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
    }
    else if (pressBack == LOW)
    {
        currentUserAction = UserAction::UA_BACK;
    }
    else if (pressLeft == LOW)
    {
        currentUserAction = UserAction::UA_LEFT;
    }
    else if (pressRight == LOW)
    {
        currentUserAction = UserAction::UA_RIGHT;
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
        else if (currentUserAction != UserAction::UA_NONE)
        {
            if (currentUserAction == UserAction::UA_LEFT)
            {
                currentHomeOption = static_cast<HomeOption>(abs(static_cast<char>(currentHomeOption) - 1) % 3);
            }
            else if (currentUserAction == UserAction::UA_RIGHT)
            {
                currentHomeOption = static_cast<HomeOption>((static_cast<char>(currentHomeOption) + 1) % 3);
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
                logIntervalTemp -= logIntervalStep;
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
                temperatureLimitTemp -= temperatureLimitStep;
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
    lcdDisplay.clear();

/*     lcdDisplay.drawString(0, 24,"Logging Interval: ");
    lcdDisplay.drawString(70, 24, String(temperatureLimit));  
    lcdDisplay.drawString(110, 24,"*C"); // ºC */
    // TOP
    lcdDisplay.setFont(ArialMT_Plain_10);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_LEFT);
    lcdDisplay.drawStringMaxWidth(0, 0, 40, enableLogging?"Log Enabled":"Log Disabled");
    if (currentHomeOption == HomeOption::HO_ENABLE_LOGGING) {
        lcdDisplay.drawRect(8, 0, 40, 20);
    }
    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawStringMaxWidth(64, 0, 40, "Change Log Interval");
    if (currentHomeOption == HomeOption::HO_EDIT_LOG_INTERVAL) {
        lcdDisplay.drawRect(40, 0, 80, 20);
    }
    lcdDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
    lcdDisplay.drawStringMaxWidth(128, 0, 40, "Change Temp. Limit");
    if (currentHomeOption == HomeOption::HO_EDIT_TEMPERATURE_LIMIT) {
        lcdDisplay.drawRect(80, 0, 120, 20);
    }
    // MIDDLE
    lcdDisplay.setFont(ArialMT_Plain_24);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawString(64, 20, String(temperatureValue) + "*C");

    lcdDisplay.setFont(ArialMT_Plain_16);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawString(64, 40, String(temperatureLimit) + "*C");

    // BOTTOM
    lcdDisplay.setFont(ArialMT_Plain_10);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
    lcdDisplay.drawString(50, 50, WiFi.status() == WL_CONNECTED ? "WI-FI On" : "WI-FI Off");

    lcdDisplay.setFont(ArialMT_Plain_10);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_LEFT);
    lcdDisplay.drawString(78, 50, "Date and time here");
    lcdDisplay.display();
}

void displayEditLogInterval()
{
    // 2 static options at the top
    // previous logInterval and new logInterval at the middle
    // date, time and wifi connectivity at the bottom
    lcdDisplay.clear();
    // MIDDLE
    lcdDisplay.setFont(ArialMT_Plain_24);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawString(64, 20, String(logIntervalTemp));
    lcdDisplay.display();
}

void displayEditTemperatureLimit()
{
    // 2 static options at the top
    // previous temperatureLimit and new temperatureLimit at the middle
    // date, time and wifi connectivity at the bottom
    lcdDisplay.clear();
    lcdDisplay.setFont(ArialMT_Plain_24);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawString(64, 20, String(temperatureLimitTemp));
    lcdDisplay.display();
}