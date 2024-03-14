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
float gain = 3; // Gain par défaut, le gain vaut 1 + R_amp/(R_mcp + R_wiper)
float ancien_gain = 3; // Variable utilisée pour checker un changement de gain

Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED); // Initilisation de l'écran

// Variables pour le KY-040
volatile unsigned int Pos_encodeur = 0; // Variable de l'encodeur
unsigned int ancienne_pos = 0; // Variable utilisée pour checker un changement de position de l'encodeur
// Variables pour le debounce du bouton
int etatBouton = HIGH;             // L'état actuel du bouton (HIGH = non pressé, LOW = pressé)
int dernierEtatBouton = HIGH;      // Le dernier état stable du bouton
unsigned long dernierTempsDebounce = 0;  // Le dernier temps où le bouton a changé d'état
unsigned long intervalleDebounce = 50;   // L'intervalle de debounce en millisecondes

// Variables pour le Flex
const float VCC = 5;      // Tension de l'Arduino
const float R_DIV = 47000.0;  // Résistance utilisée dans le pont diviseur
const float R_plat = 25000.0; // Résistance à plat
const float R_pli = 100000.0;  // Résistance à 90°


void setup() {
  // Configurations des pins
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
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

  // Configuration de l'interruption pour l'encodeur
  attachInterrupt(digitalPinToInterrupt(CLK), Encodeur_interrupt, RISING);

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
  
  /////////////////////////PARTIE FLEX
  float flex_angle = Flex_read(); // Donne la résistance du FLEX
  Serial.println(flex_angle); // A remplacer par l'envoie au bluetooth

  /////////////////////////PARTIE KYC-040
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

  /////////////////////////PARTIE BLUETOOTH
  bluetooth_gain(); // Check si le gain n'est pas mis à jour par l'application
}

// Fonction qui retourne un float ayant pour valeur la résistance du FLEX
float Flex_read() {
  float Vflex = analogRead(Flex) * VCC / 1023.0; // Lecture de la tension aux bornes du FLEX
  float Rflex = R_DIV * (VCC / Vflex - 1.0); // Calcul de la résistance du FLEX
  float angle = (Rflex - R_plat) * (90.0 - 0) / (R_pli - R_plat) + 0; // Estimation de l'angle du FLEX
  return Rflex;
}

// Pour envoyer les données au MCP41050
void EcritureSPI(uint8_t valeur, uint8_t ssPin) {
  uint8_t cmd = 0x11; // Commande d'écriture pour MCP41050
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0)); // Ouvre l'échange
  digitalWrite(ssPin, LOW); // Sélection du chip
  SPI.transfer(cmd); // Envoyer la commande d'écriture
  SPI.transfer(valeur); // Envoyer la valeur associée
  digitalWrite(ssPin, HIGH); // Désélection du chip
  SPI.endTransaction(); // Ferme l'échange
}

// Fonction d'interrupt qui modifie l'état la variable de position lorsqu'il change
void Encodeur_interrupt() {
  if (digitalRead(DT)==HIGH) { // Si l'on tourne à droite
    Pos_encodeur++;
  } else { // Si l'on tourne à gauche
    Pos_encodeur--;
  }
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
