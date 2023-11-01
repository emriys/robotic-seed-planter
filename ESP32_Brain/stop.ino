void stop() {
  //Stop wheel movement
  state = 0;
  digitalWrite(PIN_STOP, false);  // Active LOW
  digitalWrite(PIN_BRAKE, true);  // Active HIGH
  analogWrite(PIN_PWM, 0);
  digitalWrite(PIN_DIR, false);  // Forward LOW, Backward Floating/HIGH
}