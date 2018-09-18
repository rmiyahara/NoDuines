#include "Nintendo.h"
CGamecubeController controller(2); //sets D2 on arduino to read data from controller
CGamecubeConsole console(3);       //sets D3 on arduino to write data to console
Gamecube_Report_t gcc;             //structure for controller state

int multishine_counter = 0;
unsigned long long delta, last = 0;

void multishine() {
  if (gcc.ddown || multishine_counter) {
    if (multishine_counter < 67) {
      gcc.yAxis = -255;
      gcc.b = true;
    } else if (multishine_counter < 134) {
      gcc.y = true;
    } else {
      multishine_counter = 0;
    }
    multishine_counter += delta;
  }
}

void loop() {
  controller.read();
  gcc = controller.getReport();
  delta = millis() - last;
  last = millis();
  multishine();
  console.write(gcc);
}
