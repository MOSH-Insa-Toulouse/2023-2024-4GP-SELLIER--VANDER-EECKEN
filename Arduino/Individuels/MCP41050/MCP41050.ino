#include <SPI.h>

#define SS 12 // Broche 12 en tant que SS, raccordé sur SS du MCP41050

// Variables pour le MCP41050
const float R_max = 50000.0; // Résistance maximale du MCP41050
const float R_min = 0.0; // Résistance minimale du MCP41050
const float R_amp = 100000.0; // Résistance en sortie de l'amplificateur
const float R_wiper = 125.0; // Résistance interne du MCP41050
const float gain_max = 800; // 1 + R_amp/(R_min+R_wiper)
const float gain_min = 3; // 1 + R_amp/(R_max+R_wiper)
float gain = 3; // Gain par défaut, le gain vaut 1+(R_amp/R_mcp)
float ancien_gain = 3; // Variable utilisée pour checker un changement de gain

void setup(){
  // Configurations des pins
  pinMode(SS, OUTPUT);

  // Configuration du MCP41050
  digitalWrite(SS, HIGH); //SPI chip disabled
  SPI.begin();
}

void loop() {
  // Vérifier si le gain est dans l'intervalle autorisé
  if (gain < gain_min) gain = gain_min;
  else if (gain > gain_max) gain = gain_max;
  
  // Changement de la résistance du MCP41050
  if (ancien_gain != gain) { // Si détection de changement de gain
    float R_mcp = (R_amp / (gain - 1.0)) - R_wiper; // Calcul de la résistance adéquate
    int valeur_mcp = (int)((R_mcp - R_min) * 255 / (R_max - R_min) + 0.5); // Mapage entre 0 et 255 au plus proche
    EcritureSPI(valeur_mcp, SS); // Envoi vers le MCP41050
    ancien_gain = gain; // Mise à jour la valeur de l'ancien gain
  }
}

// Pour envoyer les données au MCP41050
void EcritureSPI(uint8_t valeur, uint8_t ssPin) {
  uint8_t cmd = 0x11; // Commande d'écriture pour MCP41050
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(ssPin, LOW); // Sélection du chip
  SPI.transfer(cmd); // Envoyer la commande d'écriture
  SPI.transfer(valeur); // Envoyer la valeur associée
  digitalWrite(ssPin, HIGH); // Désélection du chip
  SPI.endTransaction();
}