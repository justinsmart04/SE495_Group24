#include "HX711.h"

// HX711 circuit wiring
// pressure transducer 1
const int DOUT1 = 6;
const int SCK1 = 5;
// pressure transducer 2
const int DOUT2 = A4;
const int SCK2 = A5;
HX711 scale1;
HX711 scale2;
// variables to make baseline
long baseline1;
long baseline2;
void setup() {
  Serial.begin(57600);

  scale1.begin(DOUT1, SCK1);
  scale2.begin(DOUT2, SCK2);

  delay(2000); // let sensors settle

  baseline1 = scale1.read();
  baseline2 = scale2.read();
}
void loop() {

  if (scale1.is_ready() && scale2.is_ready()) {
    long r1 = scale1.read();
    long r2 = scale2.read();

    long d1 = r1 - baseline1;
    long d2 = r2 - baseline2;
    long dP = d1 - d2;

  Serial.print("PT1 Δ: ");
  Serial.print(d1);

  Serial.print("    PT2 Δ: ");
  Serial.print(d2);

  Serial.print("    ΔP: ");
  Serial.println(dP);
  }

  delay(500);
}