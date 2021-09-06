#include "user_interface.h"
#include "../pins/pins.h"

void userInterfaceSetup() {
    lcdDisplay.init();
    lcdDisplay.clear();
    lcdDisplay.drawString(0, 0, "Booting");
    lcdDisplay.display();
}