// Macro Code

void fc_nair_macro() {
  if (gcc.x || macro_state) {
    if (macro_state < 25) {
      gcc.y = 1;
      gcc.yAxis = 0;
    } else if (macro_state < 35) {
      gcc.y = 0;
      gcc.x = 0;
      gcc.xAxis = 128;
      gcc.yAxis = 128;
      gcc.a = 1;
    } else if (macro_state < 50) {
      gcc.yAxis = 0;
    } else {
      macro_state = 0 - delta_t;
    }
    macro_state += delta_t;
  }
}


// Timing Code in main loop

delta_t = millis() - last_t;
last_t = millis();
