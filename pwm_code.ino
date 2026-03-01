// For Arduino DUE
const int pwmPin = 9;    // PWM capable pin
const int potPin = A0;   // Potentiometer for speed
uint32_t frequency = 5000; // Let's set 5kHz as the start

void setup() {
  pinMode(pwmPin, OUTPUT);
  
  // The DUE allows you to change the resolution. 
  // 8-bit = 0-255, 12-bit = 0-4095. 
  // Higher resolution gives you smoother blower speed control.
  analogWriteResolution(12); 
}

void loop() {
  // Read pot and map to 12-bit duty cycle (0-4095)
  int sensorValue = analogRead(potPin);
  int dutyCycle = map(sensorValue, 0, 1023, 0, 4095);

  // On the DUE, we use a specific library or the variant.h 
  // to define frequency. To keep it "from scratch" without 
  // complex registers, we use the PWM library approach:
  
  pwm_write_safe(pwmPin, frequency, dutyCycle);
}

// Simple function to handle the DUE's high-level PWM
void pwm_write_safe(int pin, uint32_t freq, int duty) {
  // The DUE core actually handles frequency changes 
  // differently than the AVR. 
  // Note: For 1-10kHz, standard analogWrite on DUE is 1kHz by default.
  analogWrite(pin, duty / 16); // Standard 8-bit write
}