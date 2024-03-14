#define CLK  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define DT  4  //DT Output B
#define SW 5 // Switch connection if available

// Variables pour le KY-040
volatile unsigned int Pos_encodeur = 0; // Variable de l'encodeur
unsigned int ancienne_pos = 0; // Variable utilisée pour checker un changement de position de l'encodeur
// Variables pour le debounce du bouton
int etatBouton = HIGH;             // L'état actuel du bouton (HIGH = non pressé, LOW = pressé)
int dernierEtatBouton = HIGH;      // Le dernier état stable du bouton
unsigned long dernierTempsDebounce = 0;  // Le dernier temps où le bouton a changé d'état
unsigned long intervalleDebounce = 50;   // L'intervalle de debounce en millisecondes

void setup() {
  // Configurations des pins
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);

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

  // Logique de debounce pour le bouton
  int lectureBouton = digitalRead(SW); // On lit l'état du bouton
  if (lectureBouton != dernierEtatBouton) { // S'il est différent
    dernierTempsDebounce = millis(); // On timestamp le moment où il est appuyé
  }

  if ((millis() - dernierTempsDebounce) > intervalleDebounce) { // S'il est appuyé pendant un certain nombre de millisecondes
    if (lectureBouton != etatBouton) { // Si l'état du bouton a changé
      etatBouton = lectureBouton; // On modifie l'état
      if (etatBouton == LOW) { // Si le bouton est effectivement pressé
        // Faire action bouton pressé
      }
    }
  }
  dernierEtatBouton = lectureBouton; // On met à jour l'ancien état
}

void Encodeur_interrupt() {
  if (digitalRead(DT)==HIGH) {
    Pos_encodeur++;
  } else {
    Pos_encodeur--;
  }
}