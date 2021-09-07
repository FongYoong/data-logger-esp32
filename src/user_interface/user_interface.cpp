#include "user_interface.h"
#include "fonts/roboto_bold_8.h"
#include "fonts/roboto_bold_10.h"
#include "images/wifi_image.h"
#include "../pins/pins.h"
#include "../firebase/firebase.h"

UIPage currentUIPage = UIPage::P_HOME;
HomeOption currentHomeOption = HomeOption::HO_EDIT_LOG_INTERVAL;
UserAction currentUserAction = UserAction::UA_NONE;

unsigned long logIntervalTemp = 1000;    // Milliseconds // Store temp value
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
    lcdDisplay.clear();

/*     lcdDisplay.drawString(0, 24,"Logging Interval: ");
    lcdDisplay.drawString(70, 24, String(temperatureLimit));  
    lcdDisplay.drawString(110, 24,"*C"); // ºC */
    // TOP
    lcdDisplay.setFont(Roboto_Bold_8);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_LEFT);
    lcdDisplay.drawStringMaxWidth(0, 0, 40, enableLogging?"Disable Log":"Enable Log");
    if (currentHomeOption == HomeOption::HO_ENABLE_LOGGING) {
        lcdDisplay.drawRect(0, 0, 30, 22);
    }
    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawStringMaxWidth(64, 0, 40, "Log Interval");
    if (currentHomeOption == HomeOption::HO_EDIT_LOG_INTERVAL) {
        lcdDisplay.drawRect(50, 0, 30, 22);
    }
    lcdDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
    lcdDisplay.drawStringMaxWidth(128, 0, 40, "Temp. Limit");
    if (currentHomeOption == HomeOption::HO_EDIT_TEMPERATURE_LIMIT) {
        lcdDisplay.drawRect(98, 0, 30, 22);
    }
    // MIDDLE
    lcdDisplay.setFont(Roboto_Bold_10);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawString(32, 28, String(temperatureValue) + "*C");

    lcdDisplay.setFont(Roboto_Bold_8);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawString(32, 44, "Limit:" + String(temperatureLimit) + "*C");

    lcdDisplay.setFont(Roboto_Bold_8);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
    lcdDisplay.drawString(128, 44, String(logInterval/1000.0));

    // BOTTOM
/*     lcdDisplay.setFont(Roboto_Bold_8);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_LEFT);
    lcdDisplay.drawString(8, 50, WiFi.status() == WL_CONNECTED ? "Connnected" : "Not Connected"); */
    if (WiFi.status() == WL_CONNECTED) {
        lcdDisplay.drawXbm(16, 50, 20, 12, Wifi_Image);
    }

    lcdDisplay.setFont(Roboto_Bold_8);
    lcdDisplay.setTextAlignment(TEXT_ALIGN_RIGHT);
    lcdDisplay.drawString(128, 50, "Date/Time");
    lcdDisplay.display();
}

void displayEditLogInterval()
{
    // 2 static options at the top
    // previous logInterval and new logInterval at the middle
    // date, time and wifi connectivity at the bottom
    lcdDisplay.clear();
    // MIDDLE
    lcdDisplay.setFont(Roboto_Bold_10);

    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawString(64, 5, "Log Interval");
    lcdDisplay.display();

    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawString(64, 30, String(logIntervalTemp / 1000));
    lcdDisplay.display();
}

void displayEditTemperatureLimit()
{
    // 2 static options at the top
    // previous temperatureLimit and new temperatureLimit at the middle
    // date, time and wifi connectivity at the bottom
    lcdDisplay.clear();
    lcdDisplay.setFont(Roboto_Bold_10);

    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawString(64, 5, "Temperature Limit");
    lcdDisplay.display();

    lcdDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
    lcdDisplay.drawString(64, 30, String(temperatureLimitTemp));
    lcdDisplay.display();
}