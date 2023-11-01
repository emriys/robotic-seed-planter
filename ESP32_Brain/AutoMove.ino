void AutoMove() {
  static bool lastState = false;    // Saves the last state of the speed pin
  static unsigned long last_uS;     // The time (µs) when the speed pin changes
  static unsigned long timeout_uS;  // Timer used to determine the wheel is not spinning

  // Turn off brake and set direction to forward
  digitalWrite(PIN_STOP, true);    // Active LOW
  digitalWrite(PIN_BRAKE, false);  // Active HIGH

  // Read the current state of the input pin
  // bool state = digitalRead(PIN_SPEED);

  // Or set state to 1
  state = 1;

  // set PWM speed
  analogWrite(PIN_PWM, data);

  // Start counting immediately wheel turns on and convert to seconds
  unsigned long wheelStart = millis() * 1000;

  // Check if the pin has changed state
  // if (state != lastState) {
  while (state == 1) {
    // Calculate how long has passed since last transition
    unsigned long current_uS = micros();
    unsigned long elapsed_uS = current_uS - last_uS;

    // Calculate the frequency of the input signal
    double period_uS = elapsed_uS * 2.0;
    _freq = (1 / period_uS) * 1E6;
    Serial.print("freq: "); Serial.println(_freq);

    // Calculate the RPM
    _rpm = _freq / 45 * 60;
    Serial.print("Rpm: "); Serial.println(_rpm);

    // If RPM is excessively high then ignore it.
    if (_rpm > 5000) _rpm = 0;

    /********** Calculate wheel speed and distance covered ***********/

    // Calculate the miles per hour (mph) based on the wheel diameter or circumference
    //_mph = (WHEEL_DIAMETER_IN * PI * _rpm * 60) / 63360;
    _mph = (WHEEL_CIRCUMFERENCE_IN * _rpm * 60) / 63360;

    // Calculate the kilometers per hour (kph) based on the wheel diameter or circumference
    //_kph = (WHEEL_DIAMETER_CM * PI * _rpm * 60) / 1000;
    _kph = (WHEEL_CIRCUMFERENCE_CM * _rpm * 60) / 100000;
    Serial.print("kph: "); Serial.println(_kph);

    //Calculate centimeters per second (cmps) based on wheel diameter or circumference
    _cmps = (WHEEL_CIRCUMFERENCE_CM * _rpm / 60);
    Serial.print("cmps: "); Serial.println(_cmps);

    //Distance covered now in centimeters
    double distCovr = _cmps * ((millis() * 1000) - wheelStart);

    // Total distance now between the last drilled holes and current position in centimeters
    double distCovNow = distCovr;
    Serial.print("New Distance now: "); Serial.println(distCovNow);

    // Save the last state and next timeout time
    last_uS = current_uS;
    timeout_uS = last_uS + SPEED_TIMEOUT;
    // lastState = state;

    // Stop when total distance between the last drilled holes and current position is between seed spacing threshold
    if (13 <= distCovNow <= 15) {
      stop();
      //Add to total distance covered in centimeters
      totaldistCov += distCovNow;
      break;
    }

    //
  }
  // If too long has passed then the wheel has probably stopped
  if (micros() > timeout_uS) {
    _freq = 0;
    _rpm = 0;
    _mph = 0;
    _kph = 0;
    last_uS = micros();
  }
}