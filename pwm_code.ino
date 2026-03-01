const int pwmPin = 9;

int currentDutyCycle = 0;

int targetDutyCycle = 0;

const int accelerationStep = 10;



void setup() {

  Serial.begin(115200);

  pinMode(pwmPin, OUTPUT);

  analogWriteResolution(12);

  analogWrite(pwmPin, 0);

 

  Serial.println("RVE45 Blower Control Ready.");

  Serial.println("Enter a speed from 0 to 100:");

}



void loop() {

  if (Serial.available() > 0) {

    // The "SKIP_ALL" tells the Due to ignore the invisible Newline/Enter characters

    int inputPercentage = Serial.parseInt(SKIP_ALL);

   

    // We only update if a new number was actually found in the buffer

    // This prevents the "reset to 0" issue

    if (inputPercentage >= 0 && inputPercentage <= 100 && Serial.read() != -1) {

      inputPercentage = constrain(inputPercentage, 0, 100);

      targetDutyCycle = map(inputPercentage, 0, 100, 0, 4095);

     

      Serial.print("Target set to: ");

      Serial.print(inputPercentage);

      Serial.println("%");

    }

  }



  // Soft Start Logic (Unchanged)

  if (currentDutyCycle < targetDutyCycle) {

    currentDutyCycle += accelerationStep;

    if (currentDutyCycle > targetDutyCycle) currentDutyCycle = targetDutyCycle;

  } else if (currentDutyCycle > targetDutyCycle) {

    currentDutyCycle -= accelerationStep;

    if (currentDutyCycle < targetDutyCycle) currentDutyCycle = targetDutyCycle;

  }



  analogWrite(pwmPin, currentDutyCycle);

  delay(5);

}
