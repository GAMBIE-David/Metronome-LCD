/*
 Name:		Metronome.ino
 Created:	11/12/2017 17:33:47
 Author:	Zalman
*/

//chargement des bibliothèques
#include <LiquidCrystal.h>

//Définition des constantes
const byte LED_PIN = 13;
const int buzzer = 2;

//Constante des pin du LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Définition des variables globales
//Variables du shield LCD
int lcd_key = 0;
int adc_key_in = 0;
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

//Variable MIDI
byte midi_start = 0xfa; // pour le start de la synchro
byte midi_stop = 0xfc; // --stop
byte midi_clock = 0xf8;
byte midi_continue = 0xfb;
int play_flag = 0; // pour activer le jeu

//Variable d'etat
bool boolMIDI = false;

//Variable Metronome
int glbTemps = 0; //variable pour les temps des mesures
int glbMesure = 1;
int nbTemps = 4;
int nbMesure = 12;

// In millisecondes.
const long FREQUENCY = 700;
int tempo;
long currentTime;
long oldTime;
bool etatLed;

// Fonction lire les boutons
int read_LCD_buttons()
{
	adc_key_in = analogRead(0); // Lire les valeurs du capteurs
								// Les valeurs renvoyées sont sensés être: 0, 144, 329, 504, 741
								// Il y a une erreur possible de 50
	if (adc_key_in > 1000) return btnNONE; // Nous découpons les valeurs possibles en zone pour chaque bouton
	if (adc_key_in < 50) return btnRIGHT;
	if (adc_key_in < 250) return btnUP;
	if (adc_key_in < 450) return btnDOWN;
	if (adc_key_in < 650) return btnLEFT;
	if (adc_key_in < 850) return btnSELECT;
	return btnNONE; // On renvoie cela si l'on est au dessus de 850
}

// Fonction test de l'écran
void testEcran() {
	lcd.setCursor(0, 1); // Placer le curseur au début de la seconde ligne
	lcd_key = read_LCD_buttons(); // Lire les boutons

	switch (lcd_key) // Selon le bouton appuyer
	{
	case btnRIGHT: // Pour le bouton "Right"
	{
		lcd.print("RIGHT "); // Afficher "Right"
		break;
	}
	case btnLEFT: // Pour le bouton "left"
	{
		lcd.print("LEFT "); // Afficher "Left"
		break;
	}
	case btnUP: // Pour le bouton "Up"
	{
		lcd.print("UP "); // Afficher "Up"
		break;
	}
	case btnDOWN: // Pour le bouton "Down"
	{
		lcd.print("DOWN "); // Afficher "Down"
		break;
	}
	case btnSELECT: // Pour le bouton "Select"
	{
		lcd.print("SELECT"); // Afficher "Select"
		break;
	}
	case btnNONE: // Sinon
	{
		lcd.print("NONE "); // Afficher "None"
		break;
	}
	}

	lcd.setCursor(12, 0);
	lcd.print("A0");
	lcd.setCursor(12, 1);
	lcd.print(adc_key_in);

}

//Fonction reglage du tempo
int reglageTempo() {
	if (read_LCD_buttons() == btnUP)
	{
		tempo = tempo + 1;
		delay(200);
	}

	if (read_LCD_buttons() == btnDOWN)
	{
		tempo = tempo - 1;
		delay(200);
	}

	if (read_LCD_buttons () == btnRIGHT)
	{
		tempo = tempo +10;
		delay(200);
	}

	if (read_LCD_buttons() == btnLEFT)
	{
		tempo = tempo - 10;
		delay(200);
	}

	return tempo;

}

//fonction calcul de la fréquence par rapport au tempo
float temps (int Tempo) {
	float frequence = 0;
	float temps = 0;
	frequence = (float) 60 / tempo;
	temps = frequence * 1000;
	return temps;

}

//Fonction pour le compte des mesures
int mesure() {
	glbTemps = glbTemps + 1;
	
	if (glbTemps > nbTemps)
	{
		glbMesure = glbMesure + 1;
		glbTemps = 1;
		if (glbMesure >= nbMesure +1) 
		{
			glbMesure = 1;
			lcd.setCursor(10, 1);
			lcd.print(" ");
		}
		
	}

	//lcd.setCursor(9, 1);
	//lcd.print(glbMesure);
	return glbMesure;
}

//Fonction pour le boeuf en blues
int blues ()
{
	lcd.setCursor(12, 1);
	switch (glbMesure)
	{
	case 1:
		lcd.print("E ");
		break;
	case 5:
		lcd.print("A ");
		break;

	case 7:
		lcd.print("E ");
		break;
		
	case 9:
		lcd.print("B ");
		break;

	case 10:
		lcd.print("A ");
		break;

	case 11:
		lcd.print("E ");
		break;
		
	case 12:
		lcd.print("B ");
		break;

	default:
		break;
	}


}

//fontion pour le reglage du temps
void reglageTemps()
{
	bool menu = HIGH;
	int temps = nbTemps;
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Reglage du temps");

	while (menu)
	{
		lcd.setCursor(0, 1);
		lcd.print(temps);
		if (read_LCD_buttons () == btnUP)
		{
			temps = temps + 1;
			delay(200);
		}

		if (read_LCD_buttons() == btnDOWN)
		{
			temps = temps - 1;
			delay(200);
		}

		if (read_LCD_buttons() == btnRIGHT)
		{
			nbTemps = temps;
			lcd.setCursor(14, 1);
			lcd.print("OK");
			delay(500);
			lcd.setCursor(14, 1);
			lcd.print("  ");
			menu = LOW;
		}

		if (read_LCD_buttons () == btnLEFT)
		{
			lcd.setCursor(0, 1);
			lcd.print("annule");
			delay(500);
			menu = LOW;
		}
	}
}

//Fonction pour le reglage de la mesure
void reglageMesure()
{
	bool menu = HIGH;
	int mesure = nbMesure;
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Reglage mesure  ");

	while (menu)
	{

		lcd.setCursor(0, 1);
		lcd.print(mesure);
		if (read_LCD_buttons() == btnUP)
		{
			lcd.setCursor(0, 1);
			lcd.print("  ");
			mesure = mesure + 1;
			delay(200);
		}

		if (read_LCD_buttons() == btnDOWN)
		{
			lcd.setCursor(0, 1);
			lcd.print("  ");
			mesure = mesure - 1;
			delay(200);
		}

		if (read_LCD_buttons() == btnRIGHT)
		{
			nbMesure = mesure;
			lcd.setCursor(14, 1);
			lcd.print("OK");
			delay(500);
			lcd.setCursor(14, 1);
			lcd.print("  ");
			menu = LOW;
		}

		if (read_LCD_buttons() == btnLEFT)
		{
			lcd.setCursor(0, 1);
			lcd.print("annule");
			delay(500);
			menu = LOW;
		}
	}


}

//Gestion MIDI
void MIDI_TX(unsigned char MESSAGE, unsigned char DONNEE1, unsigned char DONNEE2, bool activation) //fonction d'envoi du message MIDI ==> voir tableau
{
	if (activation)
	{
	Serial.write(MESSAGE); //envoi de l'octet de message sur le port série
	Serial.write(DONNEE1); //envoi de l'octet de donnée 1 sur le port série
	Serial.write(DONNEE2); //envoi de l'octet de donnée 2 sur le port série
	}

}

void menuPrincipal(int navigation)
{
	bool menu = HIGH;
	int positionMenu = 0;

	if (navigation == btnSELECT)
	{
		navigation = 0;
		lcd.clear();
		//Serial.println("Entree dans le menu principal");


		do
		{
			lcd.setCursor(0, 0);
			lcd.print("Menu principal  ");
			Serial.print("Valeur de menu : ");
			Serial.println(menu);
			navigation = read_LCD_buttons();

			if (navigation == btnUP)
			{
				positionMenu = (positionMenu + 1) % 4;
				Serial.println(positionMenu);
				delay(200);
			}

			if (navigation == btnDOWN)
			{
				positionMenu = (positionMenu - 1) % 4;
				if (positionMenu == -1)
				{
					positionMenu = 3;
				}
				Serial.println(positionMenu);
				delay(200);
			}

			if (navigation == btnLEFT)
			{
				menu = LOW;
				positionMenu = 10;
				delay(200);
				lcd.clear();
			}

			if (navigation == btnRIGHT)
			{
				delay(200);
				switch (positionMenu)
				{
				case 0:
					reglageMesure();
					break;

				case 1:
					reglageTemps();
					break;

				case 2:
					break;

				case 3:
					break;
				default:
					break;
				}

			}


			switch (positionMenu)
			{
			case 0:
				lcd.setCursor(0, 1);
				lcd.print("Reglage mesure  ");
				break;

			case 1:
				lcd.setCursor(0, 1);
				lcd.print("Reglage temps   ");
				break;

			case 2:
				lcd.setCursor(0, 1);
				lcd.print("Choix du boeuf  ");
				break;

			case 3:
				lcd.setCursor(0, 1);
				lcd.print("MIDI            ");

			default:
				break;
			}

		} while (menu);
	}

}

void ecranPrincipal(int mesure, int tempo)
{
	//Affichage de l'ecran
	lcd.setCursor(0, 0);
	lcd.print("Tempo  : ");
	lcd.setCursor(0, 1);
	lcd.print("Mesure : ");
	lcd.setCursor(9, 0);
	lcd.print(tempo);
	if (tempo >= 100)
	{
		lcd.setCursor(12, 0);
		lcd.print("bpm");
	}
	else
	{
		lcd.setCursor(11, 0);
		lcd.print("bpm");
	}

	lcd.setCursor(9, 1);
	lcd.print(mesure);
	
}

void metronome(int tempo)
{
	currentTime = millis();
	if (currentTime >= (oldTime + temps(tempo)))
	{
		//envoie de la trame MIDI
		MIDI_TX(play_flag, 0xff, 0xff, true);
		MIDI_TX(midi_clock, 0xff, 0xff, true);		
		//digitalWrite(LED_PIN, (etatLed = !etatLed));
		digitalWrite(buzzer, HIGH);
		lcd.setCursor(15, 1);
		lcd.print("1");
		mesure();
		lcd.setCursor(15, 1);
		delay(100);
		digitalWrite(buzzer, LOW);
		lcd.print("0");
		Serial.print(tempo);
		oldTime = currentTime;
	}
}

void setup() {

	//initialisation de l'ecran
	lcd.begin(16, 2); // Démarrer la librairie
	lcd.clear();
	lcd.setCursor(4, 0);
	lcd.print("Metronome"); // Afficher un message simple
	for (int i = 0; i < 17; i++)
	{
		delay(220);
		lcd.setCursor(0 + i, 1);
		lcd.print(".");
	}
	lcd.clear(); // on efface l'ecran

	//Initialisation des variables
	pinMode(LED_PIN, OUTPUT);
	pinMode(buzzer, OUTPUT);
	currentTime = 0;
	oldTime = 0;
	//etatLed = LOW;
	tempo = 120;

	//Initialisation de la COM
	Serial.begin(9600);

	//Affichage de l'ecran
	lcd.setCursor(0, 0);
	lcd.print("Tempo  : ");
	lcd.setCursor(0, 1);
	lcd.print("Mesure : ");
}

void loop() {

	
	metronome(reglageTempo());
	ecranPrincipal(glbMesure, tempo);
	menuPrincipal(read_LCD_buttons());

	//blues();

	
	
}
