#include <SoftwareSerial.h>

#define txPin 6 // Broche 6 en tant que TX, raccordé sur TX du HC-05
#define rxPin 7 // Broche 7 en tant que RX, raccordé sur RX du HC-05

#define baudrate 9600 // Définition du BaudRate général
SoftwareSerial mySerial(rxPin ,txPin); // Définition du software serial

const float R_amp = 100000.0; // Résistance en sortie de l'amplificateur
const float R_wiper = 125.0; // Résistance interne du MCP41050
float gain = 3; // Gain par défaut, le gain vaut 1 + R_amp/(R_mcp + R_wiper)

void setup(){
  // Configurations des pins
  pinMode(rxPin,INPUT);
  pinMode(txPin,OUTPUT);
  
  // Configuration du BaudRate
  mySerial.begin(baudrate);
  Serial.begin(baudrate);
}

void loop(){
  bluetooth_gain(); // Check si le gain n'est pas mis à jour par l'application
  envoie_bluetooth(R_amp, R_wiper); // Envoie des données en bluetooth
}

// Pour reçevoir le gain s'il a changé
void bluetooth_gain() {
  if (mySerial.available()) { // Si des données sont à recevoir
    String inputString = mySerial.readStringUntil('\n'); // Lit la chaîne jusqu'au retour à la ligne
    int R_mcp = inputString.toInt(); // Convertit la chaîne en entier
    if (R_mcp > 0) { // Vérifie que R_mcp est un nombre positif valide
      gain = 1 + (R_amp / (R_mcp + R_wiper)); // Calcule le nouveau gain
    }
  }
}

// Pour envoyer les données en bluetooth
void envoie_bluetooth(float R_capt, float R_flex){
  // Convertit les valeurs entières en chaînes de caractères
  String R_capt_str = String(R_capt);
  String R_flex_str = String(R_flex);

  // Envoie les données au HC-05
  mySerial.println(R_capt_str); // Envoie R_capt suivi d'un retour à la ligne
  mySerial.println(R_capt_str); // Envoie R_capt suivi d'un retour à la ligne
  mySerial.println(R_flex_str); // Envoie R_flex suivi d'un retour à la ligne
}