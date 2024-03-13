#include <SoftwareSerial.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>

// Définitions des broches
#define CLK 2 // Broche 2 en tant que CLK, raccordé sur CLK du KY-040
#define DT 4 // Broche 4 en tant que DT, raccordé sur DT du KY-040
#define SW 5 // Broche 5 en tant que SW, raccordé sur SW du KY-040
#define txPin 6 // Broche 6 en tant que TX, raccordé sur TX du HC-05
#define rxPin 7 // Broche 7 en tant que RX, raccordé sur RX du HC-05
#define SS 12 // Broche 12 en tant que SS, raccordé sur SS du MCP41050
#define Ampli A0 // Broche A0 en tant que Ampli, raccordé sur la sortie du LTC1050#CN8
#define Flex A1 // Broche A1 en tant que Flex, raccordé sur le pont diviseur du capteur Flex
#define SDA A4 // Broche A4 en tant que SDA, raccordé sur SDA de l'OLED
#define SCL A5 // Broche A5 en tant que SCL, raccordé sur SCL de l'OLED

#define baudrate 9600 // Définition du BaudRate général
SoftwareSerial mySerial(rxPin ,txPin); // Définition du software serial

// Définitions pour l'OLED
#define nombreDePixelsEnLargeur 128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur 64          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED         -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED     0x3C        // Adresse de "mon" écran OLED sur le bus i2c (généralement égal à 0x3C ou 0x3D)

// Variables pour le MCP41050
const float R_max = 50000.0; // Résistance maximale du MCP41050
const float R_min = 0.0; // Résistance minimale du MCP41050
const float R_amp = 100000.0; // Résistance en sortie de l'amplificateur
const float R_wiper = 125.0; // Résistance interne du MCP41050
const float gain_max = 800; // 1 + R_amp/(R_min+R_wiper)
const float gain_min = 3; // 1 + R_amp/(R_max+R_wiper)
float gain = 3; // Gain par défaut, le gain vaut 1+(R_amp/R_mcp)
float ancien_gain = 3; // Variable utilisée pour checker un changement de gain

Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED); // Initilisation de l'écran

volatile unsigned int Pos_encodeur = 0; // Variable de l'encodeur

// Variables pour le Flex
const float VCC = 5;      // Tension de l'Arduino
const float R_DIV = 47000.0;  // Résistance utilisée dans le pont diviseur
const float R_plat = 25000.0; // Résistance à plat
const float R_pli = 100000.0;  // Résistance à 90°


void setup() {
  // Configurations des pins
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT);
  pinMode(txPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(SS, OUTPUT);
  pinMode(Ampli, INPUT);
  pinMode(Flex, INPUT);
  //pinMode(SDA, INPUT);
  //pinMode(SCL, INPUT);

  // Configuration du BaudRate
  mySerial.begin(baudrate);
  Serial.begin(baudrate);

  // Configuration de l'encodeur
  digitalWrite(CLK, HIGH); // Active la résistance de pullup
  digitalWrite(DT, HIGH); // Active la résistance de pullup
  attachInterrupt(0, Encodeur_interrupt, RISING); // Interruption de CLK déclanche la fonction

  // Configuration du MCP41050
  digitalWrite(SS, HIGH); //SPI chip disabled
  SPI.begin();

  // Configuration de l'écran OLED
  ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED);
}

void loop() {

  /////////////////////////PARTIE MCP41050

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

void Flex_read() {
  float Vflex = analogRead(Flex) * VCC / 1023.0;
  float Rflex = R_DIV * (VCC / Vflex - 1.0);
  Serial.println("Resistance: " + String(Rflex) + " ohms");

  // Use the calculated resistance to estimate the sensor's bend angle:
  float angle = map(Rflex, R_plat, R_pli, 0, 90.0);
  Serial.println("Bend: " + String(angle) + " degrees");
  Serial.println();
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

void Encodeur_interrupt() {
  if (digitalRead(DT)==HIGH) {
    Pos_encodeur++;
  } else {
    Pos_encodeur--;
  }
}
