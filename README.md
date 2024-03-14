<h1 align="center">📈 Projet capteur graphène 📉</h1>
<h2 align="center">👨‍🔬 Par VANDER-EECKEN Néo et SELLIÉ Pierre 👨‍🔬</h2>
<h3 align="center">🏫 Pour l'INSA de Toulouse, département du Génie Physique, année 2023 - 2024 🎓</h3>

<p align="center">
  <a href="http://www.gnu.org/licenses/gpl-3.0">
    <img src="https://img.shields.io/badge/License-GPL%20v3-blue.svg" alt="License: GPL v3">
  </a>
</p>

![](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-SELLIER--VANDER-EECKEN/blob/main/PCB_design.gif)

<h1 align="center">🔩 Mesure de contrainte avec capteur graphène sous Arduino 🤖</h1>


## 📝 Description

Ce projet introduit une approche novatrice pour la mesure de contrainte en exploitant les propriétés uniques du graphène à travers un capteur dédié, envisagé comme une alternative prometteuse aux jauges de contrainte conventionnelles. Grâce à l'emploi du graphène, connu pour sa sensibilité et sa flexibilité, ce capteur offre un potentiel intéressant pour une variété d'applications de mesure. Le système est bâti autour d'une plateforme Arduino, facilitant ainsi son accessibilité et sa manipulation pour tous.

Le projet inclut un circuit d'amplification pour le signal du capteur en graphène, assurant une précision accrue des mesures. Un potentiomètre digital permet d'ajuster le gain de ce circuit, offrant ainsi une calibration précise selon les besoins spécifiques de l'application. Cette approche est complétée par des modules tels que le Bluetooth, un écran OLED et un encodeur rotatoire, qui enrichissent l'expérience utilisateur et la fonctionnalité du système.

En complément de la programmation Arduino, ce projet est accompagné de fichiers KiCad détaillant le design d'un Shield UNO personnalisé. Cette approche modulaire facilite le montage et l'intégration de l'ensemble des composants, permettant ainsi une grande flexibilité dans la configuration du système.


## 🌟 Fonctionnalités

- **Capteur de contrainte en graphène** : Exploite les propriétés du graphène pour une mesure de contrainte.
- **Circuit d'amplification avec réglage** : Potentiomètre digital pour ajuster le gain.
- **Modules intégrés** : Communication Bluetooth, affichage sur écran OLED et interaction utilisateur via encodeur rotatoire.
- **Design modulaire** : Fichiers KiCad pour un Shield UNO personnalisé.


## 📦 Matériel nécessaire

- Capteur graphène (à fabriquer soi-même)
- Arduino UNO (https://store.arduino.cc/products/arduino-uno-rev3)
- Circuit d'amplification : LTC1050#CN8 + 2x100kΩ ; 10kΩ ; 1kΩ ; 3x100nF ; 1µF (https://www.analog.com/en/products/ltc1050.html)
- Potentiomètre digital : MCP41050-IP (https://www.microchip.com/en-us/product/mcp41050)
- Module Bluetooth : HC-05 (https://www.gotronic.fr/art-module-bluetooth-hc05-26097.htm)
- Écran OLED : SSD1306 (https://electropeak.com/0-96-oled-64x128-display-module)
- Encodeur rotatoire (https://www.az-delivery.de/fr/products/drehimpulsgeber-modul)
- Capteur FLEX (https://www.kiwi-electronics.com/en/short-flex-bend-sensor-2172)


## 🔧 Installation

1. Assemblez le circuit selon les schémas fournis dans les fichiers KiCad.
2. Connectez les modules et le capteur en suivant le schéma KiCad.
3. Installez l'IDE Arduino et installez la librairie "U8glib"
4. Téléchargez le code source sur votre Arduino UNO.


## 🖥️ Utilisation

Utilisez l'encodeur rotatoire pour naviguer dans le menu de l'écran OLED.


## 🤝 Contribuer

Les contributions sont bienvenues ! Si vous avez des idées d'amélioration, n'hésitez pas à forker le projet, soumettre des pull requests ou ouvrir des issues.


## 📜 Licence

Distribué sous la GNU General Public License (GPL), favorisant un écosystème ouvert et collaboratif.
