const int Flex = A1;      // Pin connected to voltage divider output

// Change these constants according to your project's design
const float VCC = 5;      // voltage at Ardunio 5V line
const float R_DIV = 47000.0;  // resistor used to create a voltage divider
const float R_plat = 25000.0; // resistance when flat
const float R_pli = 100000.0;  // resistance at 90 deg

void setup() {
  Serial.begin(9600);
  pinMode(Flex, INPUT);
}

void loop() {
  float flex_angle = Flex_read();
  Serial.println(flex_angle);
}

float Flex_read() {
  float Vflex = analogRead(Flex) * VCC / 1023.0; // Lecture de la tension aux bornes du FLEX
  float Rflex = R_DIV * (VCC / Vflex - 1.0); // Calcul de la résistance du FLEX
  return (Rflex - R_plat) * (90.0 - 0) / (R_pli - R_plat) + 0; // Estimation de l'angle du FLEX
}