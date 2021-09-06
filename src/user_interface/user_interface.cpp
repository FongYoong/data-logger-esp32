#include "user_interface.h"
#include "../pins/pins.h"

void userInterfaceSetup()
{
    lcdDisplay.init();
    lcdDisplay.clear();
    lcdDisplay.drawString(0, 0, "Booting");
    lcdDisplay.display();
}

void UILoop()
{
    int pressEnter = digitalRead(button_enter);
    int pressBack = digitalRead(button_back);
    int pressLeft = digitalRead(button_left);
    int pressRight = digitalRead(button_right);

    if (pressEnter == LOW)
    {
        vTaskDelay(DEBOUNCE_DELAY / portTICK_PERIOD_MS); // Pause the task
        //if ()
    }
    else if (pressBack == LOW) {

    }
    else if (pressLeft == LOW)
    {
        
    }
    else if (pressRight == LOW) {
        
    }
    vTaskDelay(INPUT_CHECK_INTERVAL / portTICK_PERIOD_MS); // Pause the task
}