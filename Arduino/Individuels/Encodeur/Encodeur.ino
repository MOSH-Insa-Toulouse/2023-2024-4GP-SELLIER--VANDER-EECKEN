#define CLK  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define DT  4  //DT Output B
#define Switch 5 // Switch connection if available

volatile unsigned int Pos_encodeur = 0; // Variable de l'encodeur
unsigned int ancienne_pos = 0; // Variable utilisée pour checker un changement de position de l'encodeur

void setup() {
  // Configurations des pins
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);

  // Configuration du BaudRate
  Serial.begin(9600);

  // Configuration de l'interruption pour l'encodeur
  attachInterrupt(digitalPinToInterrupt(CLK), Encodeur_interrupt, RISING);
}

void loop(){
  noInterrupts(); // Désactive les interruptions pour une lecture atomique
  unsigned int posActuelle = Pos_encodeur; // Copie atomique de Pos_encodeur
  interrupts(); // Réactive les interruptions

  if (posActuelle  != ancienne_pos){ // Si l'on détecte un changement
    if (posActuelle  > ancienne_pos) { // Si c'est vers la droite
      // Faire action droite
    }
    else { // Si c'est vers la gauche
      // Faire action gauche
    }
    ancienne_pos = posActuelle; // Mise à jour de l'ancienne position
  }
}

void Encodeur_interrupt() {
  if (digitalRead(DT)==HIGH) {
    Pos_encodeur++;
  } else {
    Pos_encodeur--;
  }
}