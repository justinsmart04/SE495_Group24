// --- PIN DEFINITIONS ---
const int pwmPin = 9; 
const int tachoPin = 2; 
const int flowPin = A1; 

// --- SENSOR DOCUMENTATION CONSTANTS ---
#define RANGE             150    // Measurement Range
#define ZEROVOLTAGE       0.5    // Zero Voltage 
#define FULLRANGEVOLTAGE  4.5    // Full scale voltage
#define VREF              3.3    // Arduino Due Reference Voltage is 3.3V

// --- HARDWARE CONSTANTS ---
const float dividerRatio = 0.6875; // 10k/22k Divider ratio
const float slmToFpsConst = 0.408; // 1.625" ID Tube constant

// PWM & Ramping Logic
int currentDutyCycle = 0;
int targetDutyCycle = 0;
const int accelerationStep = 5; 

// Tachometer Variables
volatile unsigned long pulseCount = 0; 
unsigned long lastUpdate = 0; 

void countPulse() { pulseCount++; }

void setup() {
  Serial.begin(115200); 
  
  pinMode(pwmPin, OUTPUT);
  // Due has 12-bit PWM capability (0-4095)
  analogWriteResolution(12); 
  analogWrite(pwmPin, 0); 
  
  pinMode(tachoPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(tachoPin), countPulse, FALLING);

  analogReadResolution(12);  // 0-4095
  
  Serial.println("==============================================");
  Serial.println(" RVE45 TEST RIG: LINEAR MAPPING ACTIVE");
  Serial.println(" Mapping: 0=Brake | 1-100 = 10%-90% Duty");
  Serial.println("==============================================");
}

void loop() {
  // 1. User Input Handling
  if (Serial.available() > 0) {
    int input = Serial.parseInt(SKIP_ALL); 
    
    if (input == 0) {
      // Per datasheet, 0% initiates braking 
      targetDutyCycle = 0;
      Serial.println("\n>>> Command: 0% (Braking/Off)");
    } 
    else if (input >= 1 && input <= 100) {
      // Map 1-100 input to the 10%-90% linear range of the 12-bit duty cycle.
      // 10% of 4095 ≈ 410 (Min Speed: 3,000 RPM) [cite: 77]
      // 90% of 4095 ≈ 3685 (Max Speed: 36,500 RPM) [cite: 94]
      targetDutyCycle = map(input, 1, 100, 410, 3685);
      
      Serial.print("\n>>> Input: "); Serial.print(input); 
      Serial.print("% | Target Duty: "); Serial.println(targetDutyCycle);
    }
    
    while(Serial.available() > 0) Serial.read(); 
  }

  // 2. Safe Soft Start (Ramping)
  if (currentDutyCycle < targetDutyCycle) {
    currentDutyCycle += accelerationStep;
    if (currentDutyCycle > targetDutyCycle) currentDutyCycle = targetDutyCycle;
  } 
  else if (currentDutyCycle > targetDutyCycle) {
    currentDutyCycle -= accelerationStep;
    if (currentDutyCycle < targetDutyCycle) currentDutyCycle = targetDutyCycle;
  }
  
  // Constrain to 12-bit limits and write to blower
  analogWrite(pwmPin, constrain(currentDutyCycle, 0, 4095));

  // 3. Data Update (Every 1 Second)
  if (millis() - lastUpdate >= 1000) {
    
    // --- TACHOMETER ---
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();
    // Tacho frequency is n/60; 2 pulses per revolution for most ebmpapst fans [cite: 116, 117]
    float rpm = (pulses * 60.0) / 2.0;

    // --- MASS FLOW ---
    float rawValue = analogRead(flowPin);
    float voltageAtPin = (rawValue * VREF) / 4096.0; 
    float sensorValue = voltageAtPin / dividerRatio; 
    float slm = RANGE * (sensorValue - ZEROVOLTAGE) / (FULLRANGEVOLTAGE - ZEROVOLTAGE);
    
    if (slm < 0.2) slm = 0; 

    // --- VELOCITY ---
    float velocityFps = slm * slmToFpsConst;

    // --- OUTPUT ---
    Serial.print("RPM: "); Serial.print((int)rpm);
    Serial.print(" | FLOW: "); Serial.print(slm, 2); Serial.print(" SLM");
    Serial.print(" | DUTY: "); Serial.print(currentDutyCycle);
    Serial.print(" ("); Serial.print((currentDutyCycle / 40.95), 1); Serial.println("%)");
    
    lastUpdate = millis();
  }
  delay(2); 
}