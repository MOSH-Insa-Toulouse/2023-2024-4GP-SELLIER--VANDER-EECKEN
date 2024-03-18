#include <SPI.h>

#define SS 12 // Broche 12 en tant que SS, raccordé sur SS du MCP41050

// Variables pour le MCP41050
const double R_wiper = 125.0; // Résistance interne du MCP41050
const double R_max = 50000.0; // Résistance maximale du MCP41050
const double R_min = R_wiper; // Résistance minimale du MCP41050
const double R_amp = 100000.0; // Résistance en sortie de l'amplificateur
const double gain_max = 400; // 1 + R_amp/(R_min+R_wiper)
const double gain_min = 3; // 1 + R_amp/(R_max+R_wiper)
double R_mcp = R_max - R_wiper; // Résistance du MCP41050, par défaut maximale
double gain = 3; // Gain par défaut, le gain vaut 1 + R_amp/(R_mcp + R_wiper)
double ancien_gain = 3; // Variable utilisée pour checker un changement de gain

void setup(){
  // Configurations des pins
  pinMode(SS, OUTPUT);

  // Configuration du BaudRate
  Serial.begin(9600);

  // Configuration du MCP41050
  digitalWrite(SS, HIGH); // Désactivation du chip SPI
  SPI.begin(); // Initialisation du bus SPI.
  SPI.setDataMode(SPI_MODE0); // Configuration du mode SPI.
  SPI.setBitOrder(MSBFIRST); // Configuration de l'ordre des bits.
}

void loop() {
  // Changement de la résistance du MCP41050
  if (ancien_gain != gain) { // Si détection de changement de gain
    R_mcp = (R_amp / (gain - 1.0)) - R_wiper; // Calcule la nouvelle résistance R_mcp
    int valeur_mcp = int((gain_max - gain) / (gain_max - gain_min) * 255 + 0.5); // Mapage entre 0 et 255 au plus proche
    EcritureSPI(valeur_mcp, SS); // Envoi vers le MCP41050
    ancien_gain = gain; // Mise à jour la valeur de l'ancien gain
    Serial.println(("Nouveau gain : " + String(gain)).c_str());
    Serial.println(("Nouvel R_mcp : " + String(R_mcp)).c_str());
    Serial.println(("Valeur envoyée au MCP : " + String(valeur_mcp)).c_str());
  }
  changement_gain(); // Check si le gain n'est pas mis à jour par l'application
}

// Pour envoyer les données au MCP41050
void EcritureSPI(uint8_t valeur, uint8_t ssPin) {
  uint8_t cmd = 0x11; // Commande d'écriture pour MCP41050
  digitalWrite(ssPin, LOW); // Sélection du chip
  SPI.transfer(cmd); // Envoyer la commande d'écriture
  SPI.transfer(valeur); // Envoyer la valeur associée
  digitalWrite(ssPin, HIGH); // Désélection du chip
}

// Pour reçevoir le gain s'il a changé
void changement_gain() {
  if (Serial.available()) { // Si des données sont à recevoir
    String inputString = Serial.readStringUntil('\n'); // Lit la chaîne jusqu'au retour à la ligne
    int gain_souhaite = inputString.toInt(); // Convertit la chaîne en entier
    if ((gain_souhaite >= gain_min) && (gain_souhaite <= gain_max)) { // Vérifie que le gain demandé est un nombre positif valide
      gain = gain_souhaite; // Mets à jour le gain
    }
  }
}
