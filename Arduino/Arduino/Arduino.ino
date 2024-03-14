#include <SoftwareSerial.h>
#include <SPI.h>
#include "U8glib.h"

// Définitions des broches
#define CLK 2 // Broche 2 en tant que CLK, raccordé sur CLK du KY-040
#define DT 4 // Broche 4 en tant que DT, raccordé sur DT du KY-040
#define SW 5 // Broche 5 en tant que SW, raccordé sur SW du KY-040
#define txPin 6 // Broche 6 en tant que TX, raccordé sur TX du HC-05
#define rxPin 7 // Broche 7 en tant que RX, raccordé sur RX du HC-05
#define SS 12 // Broche 12 en tant que SS, raccordé sur SS du MCP41050
#define Ampli A0 // Broche A0 en tant que Ampli, raccordé sur la sortie du LTC1050#CN8
#define Flex A1 // Broche A1 en tant que Flex, raccordé sur le pont diviseur du capteur Flex

#define baudrate 9600 // Définition du BaudRate général
SoftwareSerial mySerial(rxPin ,txPin); // Définition du software serial

// Variables pour le MCP41050
const float R_max = 50000.0; // Résistance maximale du MCP41050
const float R_min = 0.0; // Résistance minimale du MCP41050
const float R_amp = 100000.0; // Résistance en sortie de l'amplificateur
const float R_wiper = 125.0; // Résistance interne du MCP41050
const float gain_max = 800; // 1 + R_amp/(R_min+R_wiper)
const float gain_min = 3; // 1 + R_amp/(R_max+R_wiper)
float R_mcp = R_max + R_wiper; // Résistance du MCP41050, par défaut maximale
float gain = 3; // Gain par défaut, le gain vaut 1 + R_amp/(R_mcp + R_wiper)
float ancien_gain = 3; // Variable utilisée pour checker un changement de gain

// Initilisation de l'écran
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

// Variables pour le KY-040
volatile unsigned int Pos_encodeur = 0; // Variable de l'encodeur
unsigned int ancienne_pos = 0; // Variable utilisée pour checker un changement de position de l'encodeur
// Variables pour le debounce du bouton
int etatBouton = HIGH; // L'état actuel du bouton (HIGH = non pressé, LOW = pressé)
int dernierEtatBouton = HIGH; // Le dernier état stable du bouton
unsigned long dernierTempsDebounce = 0; // Le dernier temps où le bouton a changé d'état
unsigned long intervalleDebounce = 50; // L'intervalle de debounce en millisecondes

// Variables pour le Flex
const float VCC = 5; // Tension de l'Arduino
const float R_DIV = 47000.0; // Résistance utilisée dans le pont diviseur
const float R_plat = 25000.0; // Résistance à plat
const float R_pli = 100000.0; // Résistance à 90°
float R_flex; // Résistance actuelle du FLEX

// Variable pour le capteur
const float R1 = 100000.0;
const float R2 = 100000.0;
const float R4 = 10000.0;
float R_capt;

// Variables pour le menu
const int nbr_items = 3; // Nombre d'items dans le menu principal
const int taille_max_items = 20; // Taille maximale des items
int item_actuel = 0; // Item du menu séléctionné
int ecran_actuel = 0; // 0 = Menu, 1 = Amplification, 2 = Valeurs

// Icône paramètre
const unsigned char bitmap_icone_param [] PROGMEM = {
  0x00, 0x00, 0x01, 0x80, 0x03, 0xc0, 0x16, 0x68, 0x3c, 0x3c, 0x21, 0x84, 0x33, 0xcc, 0x16, 0x68, 
	0x16, 0x68, 0x33, 0xcc, 0x21, 0x84, 0x3c, 0x3c, 0x16, 0x68, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00
};

// Icône graphique
const unsigned char bitmap_icone_graph [] PROGMEM = {
  0x00, 0x00, 0x80, 0x00, 0x84, 0x00, 0xa4, 0x00, 0x82, 0x00, 0xc2, 0x00, 0xc1, 0x00, 0x81, 0x00, 
	0x80, 0x00, 0x80, 0x82, 0x80, 0x82, 0x80, 0x00, 0x80, 0x44, 0x80, 0x28, 0x80, 0x00, 0x00, 0x00
};

// Icône reset
const unsigned char bitmap_icone_reset [] PROGMEM = {
  0x00, 0xc0, 0x00, 0xf0, 0xf0, 0x18, 0x30, 0x04, 0x70, 0x06, 0x50, 0x02, 0xc0, 0x03, 0xc0, 0x03, 
	0xc0, 0x03, 0xc0, 0x03, 0x40, 0x02, 0x60, 0x06, 0x20, 0x04, 0x18, 0x18, 0x0f, 0xf0, 0x03, 0xc0
};

// Tableau des bitmaps
const unsigned char* bitmap_icones[3] = {
  bitmap_icone_param,
  bitmap_icone_graph,
  bitmap_icone_reset
};

// Barre de défilement
const unsigned char bitmap_barre [] PROGMEM = {
  0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 
  0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 
  0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 
  0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00
};

// Entourer l'élément sélectionné
const unsigned char bitmap_item_actuel [] PROGMEM = {
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
  0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0
};

// Array qui contient les noms des menus
char items_menu [nbr_items] [taille_max_items] = {
  {"Amplification"}, 
  {"Valeurs"},
  {"Reset"}
 };

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

  // Configuration du BaudRate
  mySerial.begin(baudrate);
  Serial.begin(baudrate);

  // Configuration de l'interruption pour l'encodeur
  attachInterrupt(digitalPinToInterrupt(CLK), Encodeur_interrupt, RISING);

  // Configuration du MCP41050
  digitalWrite(SS, HIGH); //SPI chip disabled
  SPI.begin();

  // Configuration de l'écran OLED
  u8g.setColorIndex(1);  // Défini les écritures en blanc
}

void loop() {

  /////////////////////////PARTIE MCP41050/////////////////////////

  // Vérifier si le gain est dans l'intervalle autorisé
  if (gain < gain_min) gain = gain_min;
  else if (gain > gain_max) gain = gain_max;
  
  // Changement de la résistance du MCP41050
  if (ancien_gain != gain) { // Si détection de changement de gain
    R_mcp = (R_amp / (gain - 1.0)) - R_wiper; // Calcul de la résistance adéquate
    int valeur_mcp = (int)((R_mcp - R_min) * 255 / (R_max - R_min) + 0.5); // Mapage entre 0 et 255 au plus proche
    EcritureSPI(valeur_mcp, SS); // Envoi vers le MCP41050
    ancien_gain = gain; // Mise à jour la valeur de l'ancien gain
  }

  /////////////////////////PARTIE CAPTEUR/////////////////////////

  float V_ADC = analogRead(Ampli);
  R_capt = ((1+R2/(R_mcp+R_wiper))*R1*VCC/V_ADC)-R1-R4;
  
  /////////////////////////PARTIE FLEX/////////////////////////

  R_flex = Flex_read(); // Donne la résistance du FLEX
  // METTRE L'ENVOI DU BLUETOOTH

  /////////////////////////PARTIE KYC-040/////////////////////////

  noInterrupts(); // Désactive les interruptions pour une lecture atomique
  unsigned int posActuelle = Pos_encodeur; // Copie atomique de Pos_encodeur
  interrupts(); // Réactive les interruptions

  // Logique de navigation dans le menu avec l'encodeur
  if (ecran_actuel == 0) { // La navigation dans le menu est active seulement si ecran_actuel == 0
    if (posActuelle != ancienne_pos) { // Si l'on détecte un changement
      if (posActuelle > ancienne_pos) { // Rotation vers le haut
        item_actuel--;
        if (item_actuel < 0) {
          item_actuel = nbr_items - 1; // Boucle au dernier élément si on dépasse le premier
        }
      } else { // Rotation vers le bas
        item_actuel++;
        if (item_actuel >= nbr_items) {
          item_actuel = 0; // Boucle au premier élément si on dépasse le dernier
        }
      }
      ancienne_pos = posActuelle; // Mise à jour de l'ancienne position
    }
  }

  int lectureBouton = digitalRead(SW); // On lit l'état du bouton
  if (lectureBouton != dernierEtatBouton) { // S'il est différent
    dernierTempsDebounce = millis(); // On timestamp le moment où il est appuyé
  }
  if ((millis() - dernierTempsDebounce) > intervalleDebounce) { // S'il est appuyé pendant un certain nombre de millisecondes
    if (lectureBouton != etatBouton) { // Si l'état du bouton a changé
      etatBouton = lectureBouton; // On modifie l'état
      if (etatBouton == LOW) { // Si le bouton est effectivement pressé
        if (ecran_actuel == 0) { // Si on est sur le menu
          if (item_actuel == 2) { //Si on est sur le bouton reset
            asm volatile ("  jmp 0"); // Reset de la carte
          } else {
            ecran_actuel = item_actuel + 1; // On va dans l'écran de l'item séléctionné
          }
        } else { // On revient au menu principal
          ecran_actuel = 0;
        }
      }
    }
  }
  dernierEtatBouton = lectureBouton; // On met à jour l'ancien état

  /////////////////////////PARTIE BLUETOOTH/////////////////////////

  bluetooth_gain(); // Check si le gain n'est pas mis à jour par l'application

  /////////////////////////PARTIE OLED/////////////////////////

  menu_page(item_actuel, gain, R_capt, R_flex); // On écrit sur le menu
}

// Fonction qui retourne un float ayant pour valeur la résistance du FLEX
float Flex_read() {
  float Vflex = analogRead(Flex) * VCC / 1023.0; // Lecture de la tension aux bornes du FLEX
  float R_flex = R_DIV * (VCC / Vflex - 1.0); // Calcul de la résistance du FLEX
  //float angle = (R_flex - R_plat) * (90.0 - 0) / (R_pli - R_plat) + 0; // Estimation de l'angle du FLEX
  return R_flex;
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

void menu_page(int item_actuel, int gain, float R_capt, float R_flex){
  u8g.firstPage(); // Nécessaire
  do {
    // Ecran menu
    if (ecran_actuel == 0) {
      // Tour d'item sélectionné
      u8g.drawBitmapP(0, 64/nbr_items * item_actuel + item_actuel, 128/8, 21, bitmap_item_actuel);

      // Police
      u8g.setFont(u8g_font_7x14);

      // Amplification
      u8g.drawStr(25, 15, items_menu[0]); 
      u8g.drawBitmapP( 4, 2, 16/8, 16, bitmap_icones[0]);          

      // Valeurs
      u8g.drawStr(25, 15+20+2, items_menu[1]);   
      u8g.drawBitmapP( 4, 24, 16/8, 16, bitmap_icones[1]);     

      // Reset    
      u8g.drawStr(25, 15+20+20+2+2, items_menu[2]);   
      u8g.drawBitmapP( 4, 46, 16/8, 16, bitmap_icones[2]);  

      // Barre de défilement
      u8g.drawBitmapP(128-8, 0, 8/8, 64, bitmap_barre);

      // Emplacement de la barre de défilement
      u8g.drawBox(125, 64/nbr_items * item_actuel, 3, 64/nbr_items);           
    }

    // Ecran amplification
    if (ecran_actuel == 1) {
      // Tour d'item sélectionné
      u8g.drawBitmapP(0, 44, 128/8, 21, bitmap_item_actuel);

      // Police
      u8g.setFont(u8g_font_7x14);

      // Amplification
      u8g.drawStr(25, 15, items_menu[0]); 
      u8g.drawBitmapP(2, 2, 16/8, 16, bitmap_icones[0]);

      // Gain
      u8g.drawStr(4, 37, ("Gain : " + String(gain)).c_str()); 

      // Retour
      u8g.drawStr(25, 59, "Retour");   
      u8g.drawBitmapP(4, 46, 16/8, 16, bitmap_icones[2]);
    }

    // Ecran valeurs
    if (ecran_actuel == 2) {
      int R_flex_int = static_cast<int>((R_flex + 0.5)/1000); // Convertit à l'entier le plus proche
      int R_capt_int = static_cast<int>(R_capt + 0.5); // Convertit à l'entier le plus proche

      // Tour d'item sélectionné
      u8g.drawBitmapP(0, 44, 128/8, 21, bitmap_item_actuel);

      // Police
      u8g.setFont(u8g_font_7x14);

      // Flex
      u8g.drawStr(4, 15, ("Flex : " + String(R_flex_int) + " kOmhs").c_str()); 
      // Capt
      u8g.drawStr(4, 37, ("Capt : " + String(R_capt_int) + " Omhs").c_str());

      // Retour
      u8g.drawStr(25, 59, "Retour");   
      u8g.drawBitmapP(4, 46, 16/8, 16, bitmap_icones[2]);
    }
  } while (u8g.nextPage()); // Nécessaire
}