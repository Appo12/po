/*
Les fonctions créer : 

  Pour le choix du monstre : 
    bougerFlecheEgg()
    choix_egg() 

  Pour la page d'acceuil, le choix du menu : 

    menu()
    bougerFleche()
    choixMenu()
    
  Pour la fenêtre de stat : 

    fenetreStat()
    manger()

  Pour jouer : 
    jouer()
    jeuxDino()
    sautPersonnage()
    nuage()
*/

#include <Preferences.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include "Monstre.h"
#include "Images.h"
#include "Demarrage.h"

Monstre truc;  // truc de type monstre (pour les stats)

bool choix_monstre = false;
bool placement_fleche = false;

int onglet = 1;
bool statOk = true;

// On assossie le numéro de nos pin connecter a nos bouton au nomn de nos boutons
int boutonMilieu = 17;
int boutonGauche = 18;
int boutonDroite = 4;


//Mes variable qui vont me permttre de calculer le temps qui passe
int secondeEnCicle = 67;

Preferences preferences;
float tempsTamaFinBoucle;
int secondesTama;
int minutesTama;
float tempsDebutTama;
float finBoucleapresSauvegarde;
int debutBoucle;

int choixMonstreAvant = 1;
//Mes variable qui vont me permttre de calculer le temps qui passe en jeu
float tempsJeuFinBoucle = 0;
float tempsJeuDebut = 0;
int secondesJeu = 0;
int minutesJeu = 0;

int positionXfleche = 4;  //position de la fleche pour la bouger

int posXmonstre = 64;  //position du monstre pour le bouger en jeu
int posYmonstre = 11;


// variable jeu du dino \\

int posXnuage1 = 128 + 20;  //Position de départ de nos nuages 128 étant la taille de l'écran
int posXnuage2 = 128 + 74;
int posXnuage3 = 128 + 128;

int posXobstacle = 128 + 1;
int posXobstacle1 = 128 + 41;
int posXobstacle2 = 128 + 91;
int score = 0;  // score en jeu

bool doitDescendre = false;  // Si notre personnage a fini sont saut dans la fonctions sautPersonnage()
bool doitSauter = false;     // Si notre personnage est en cours de saut dans la fontion sautPersonnages()

bool finJeu = false;

// fin variables jeux du dino \\


Adafruit_SSD1306 oled(128, 64, &Wire, -1);
void setup() {
  Serial.begin(9600);

  pinMode(boutonMilieu, INPUT_PULLUP);
  pinMode(boutonGauche, INPUT_PULLUP);
  pinMode(boutonDroite, INPUT_PULLUP);

  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.setTextColor(WHITE);
  oled.clearDisplay();
  preferences.begin("my-app", false);
  debutBoucle = 1;
  choixMonstreAvant = 1;
  delay(1000);

  oled.display();
}

void loop() {
  //--------------------------------------------- Temps et récuperation du temps qui passe quand appareil éteint.
  oled.clearDisplay();

  Serial.println(debutBoucle);

  if (debutBoucle == 1) {
    debutBoucle = 2;
    Serial.println("OK1");
    tempsTamaFinBoucle = preferences.getFloat("tempsFin", 0);
    Serial.println("OK2");
    finBoucleapresSauvegarde = tempsTamaFinBoucle;
    secondesTama = preferences.getInt("sec", 0);
    Serial.println("OK3");
    minutesTama = preferences.getInt("min", 0);
    Serial.println("OK4");
    tempsDebutTama = preferences.getFloat("debut", 0);
    Serial.println("OK5");
    Serial.println("-------Lancement-------");
    Serial.println(secondesTama);
    Serial.println(minutesTama);
    Serial.println(tempsTamaFinBoucle);
    Serial.println(tempsDebutTama);
    Serial.println(finBoucleapresSauvegarde);
    Serial.println("-------Fin lancement-------");
  }

  if (secondesTama >= 60) {
    tempsDebutTama = finBoucleapresSauvegarde + millis();
    minutesTama += 1;
  }
  secondesTama = (tempsTamaFinBoucle - tempsDebutTama) / 1000;
  menu();
  tempsTamaFinBoucle = finBoucleapresSauvegarde + millis();

  //-------------------------------------------
  finCicle();
  mettreStatsOk();
  oled.display();
  preferences.putInt("sec", secondesTama);
  preferences.putInt("min", minutesTama);
  preferences.putFloat("tempsFin", tempsTamaFinBoucle);
  preferences.putFloat("debut", tempsDebutTama);

}

// fonctions écran principale //

void menu() {
  oled.drawBitmap(0,15,fondMenu,128,49, WHITE);
  oled.drawBitmap(0, 0, barreTache, 128, 15, WHITE);
  oled.drawBitmap(112, 1, soin, 15, 14, WHITE);
  oled.drawBitmap(64 - (truc.largeur / 2), 64 - truc.hauteur, truc.image, truc.largeur, truc.hauteur, WHITE);

  oled.drawBitmap(positionXfleche, 16, fleche, 5, 5, WHITE);
  bougerFlecheMenu();
  choixMenu();
}

void bougerFlecheMenu() {
  delay(150);
  if (digitalRead(boutonGauche) == 0) {
    if (positionXfleche != 4) {
      if (positionXfleche == 36) {
        positionXfleche -= 32;
      } else {
        positionXfleche -= 16;
      }
    }
  }

  if (digitalRead(boutonDroite) == 0) {
    if (positionXfleche != 116) {
      if (positionXfleche == 4) {
        positionXfleche += 32;
      } else {
        positionXfleche += 16;
      }
    }
  }
}

void choixMenu() {
  if (digitalRead(boutonMilieu) == 0) {

    if (positionXfleche == 68)  // position manger
    {
      Serial.println("lancer le mode manger");
      manger();
    }
    //-------------------------------------------------------
    if (positionXfleche == 52)  // position jeu
    {
      oled.clearDisplay();
      Serial.println("lancer le mode jeu");
      if (truc.fatigue >= 80 || truc.hunger >= 80) {
        oled.setCursor(0, 18);
        oled.print(truc.nom);
        oled.println(" n'est pas assez en forme pour jouer !");
      } else {
        jouer();
        score = 0;
      }
    }

    if (positionXfleche == 116)  // position soin
    {
      Serial.println("lancer le mode soin");
    }

    if (positionXfleche == 84)  // position dormir
    {
      Serial.println("lancer le mode dormir");
      truc.fatigue -= 100;
    }

    if (positionXfleche == 4)  // postion fenetre stat
    {
      Serial.println("ouvrir stat");
      fenetreStat();
    }

    if (positionXfleche == 100)  // position habitat
    {
      Serial.println("ouvrir les habitats");
    }
    if (positionXfleche == 36)  // position netoyage
    {
      Serial.println("lancer le mode nettoyage");
      truc.proprete += 100;
    }
  }
}

void mettreStatsOk() {
  // fatigue

  if (truc.fatigue > 100) {
    truc.fatigue = 100;
  }
  if (truc.fatigue < 0) {
    truc.fatigue = 0;
  }


  // faim
  if (truc.hunger > 100) {
    truc.hunger = 100;
  }
  if (truc.hunger < 0) {
    truc.hunger = 0;
  }

  // amusement
  if (truc.fun > 100) {
    truc.fun = 100;
  }
  if (truc.fun < 0) {
    truc.fun = 0;
  }

  // affection
  if (truc.affection > 100) {
    truc.affection = 100;
  }
  if (truc.affection < 0) {
    truc.affection = 0;
  }

  //vie
  if (truc.health > 100) {
    truc.health = 100;
  }
  if (truc.health < 0) {
    truc.health = 0;
  }

  //proprete

  if (truc.proprete > 100) {
    truc.proprete = 100;
  }
  if (truc.proprete < 0) {
    truc.proprete = 0;
  }
}

void finCicle()  // Permet de reduire les stats au bout d'un certain temps
{

  if ((secondesTama % 15) == 0) {
    if (secondeEnCicle != secondesTama) {

      if (truc.fatigue == 100 || truc.hunger == 100 || truc.proprete == 0) {
        truc.health -= 10;
      }

      secondeEnCicle = secondesTama;
      truc.hunger += 5;
      truc.fatigue += 5;
      truc.fun -= 5;
      truc.proprete -= 5;
      truc.affection -= 5;
    }
  }
}

// Fin fonctions écran principale//


// Fonctions fenêtre stats //

void fenetreStat() {
  do {
    oled.clearDisplay();

    if (digitalRead(boutonDroite) == 0) {
      if (onglet == 3) {
        onglet = 1;
      } else {
        onglet += 1;
      }
    }
    if (digitalRead(boutonGauche) == 0) {
      if (onglet == 1) {
        onglet = 3;
      } else {
        onglet -= 1;
      }
    }

    // première et deuxième page
    if (onglet == 1 || onglet == 2) {
      oled.setCursor(4, 4);
      oled.println(truc.nom);

      oled.setCursor(85, 4);
      oled.print(truc.age);
      oled.println(" ans");

      oled.drawLine(0, 15, 128, 15, WHITE);
    }

    // Première page
    if (onglet == 1) {
      oled.setCursor(0, 20);
      oled.print("vie");
      oled.drawRect(62, 22, (int)map(truc.health, 0, 100, 0, 46), 2, WHITE);
      oled.drawRect(60, 20, 50, 6, WHITE);

      oled.setCursor(0, 36);
      oled.print("affection");
      oled.drawRect(62, 38, (int)map(truc.affection, 0, 100, 0, 46), 2, WHITE);
      oled.drawRect(60, 36, 50, 6, WHITE);

      oled.setCursor(0, 52);
      oled.print("faim");
      oled.drawRect(62, 54, (int)map(truc.hunger, 0, 100, 0, 46), 2, WHITE);
      oled.drawRect(60, 52, 50, 6, WHITE);
    }



    // Deuxième page
    if (onglet == 2) {
      oled.setCursor(0, 20);
      oled.print("fun");
      oled.drawRect(62, 22, (int)map(truc.fun, 0, 100, 0, 46), 2, WHITE);
      oled.drawRect(60, 20, 50, 6, WHITE);

      oled.setCursor(0, 36);
      oled.print("fatigue");
      oled.drawRect(62, 38, (int)map(truc.fatigue, 0, 100, 0, 46), 2, WHITE);
      oled.drawRect(60, 36, 50, 6, WHITE);

      oled.setCursor(0, 52);
      oled.print("proprete");
      oled.drawRect(62, 54, (int)map(truc.proprete, 0, 100, 0, 46), 2, WHITE);
      oled.drawRect(60, 52, 50, 6, WHITE);
    }

    // Troisième page
    if (onglet == 3) {
      oled.setCursor(4, 4);
      oled.println("temps en jeu : ");
      oled.setCursor(0, 20);
      oled.print((int)minutesTama / 60);
      oled.println(" heures ");
      oled.setCursor(0, 36);
      oled.print(minutesTama % 60);
      oled.println(" minutes ");
      oled.setCursor(0, 52);
      oled.print((int)secondesTama);
      oled.println(" secondes ");

      oled.drawLine(0, 15, 128, 15, WHITE);
    }


    // oled.setCursor(, 30);
    // oled.println(truc.type);
    oled.display();
    delay(200);
  } while (digitalRead(boutonMilieu) == 1);
  onglet = 1;
}

// Fin fonction fennetre de stat //

// Fonctions pour le jeux du Dino //


void jouer() {

  oled.clearDisplay();
  delay(1000);
  tempsJeuDebut = millis();  //permetra de calculer le temps de jeu, pour ca on commence en mettant un compteur au debut du jeu
  posYmonstre = 55 - 15;     // donne la position du monstre dan sle jeu en abscice

  while (digitalRead(boutonDroite) == 1 && finJeu == false)  // si le bouton de droite est activé alors le jeu s'arrete
  {
    jeuDino();  // on lance le jeu
    if (((20 <= posXobstacle && posXobstacle <= 31) && posYmonstre >= 35) || ((20 <= posXobstacle1 && posXobstacle1 <= 31) && posYmonstre >= 35) || ((20 <= posXobstacle2 && posXobstacle2 <= 31) && posYmonstre >= 35)) {
      delay(1000);
      finJeu = true;
    }
  }

  statOk = false;      // quand le jeu est fini, il faut mettre a jour les stats, donc la elles ne sont pas misent a jour donc false
  doitSauter = false;  // au cas ou le personnage est en cours de saut alors on re!met les saute et descendre a false
  doitDescendre = false;
  finJeu = false;
  posXobstacle = 128 + 1;  // donne la position de départ des obstacles
  posXobstacle1 = 128 + 41;
  posXobstacle2 = 128 + 91;
  oled.clearDisplay();

  do  // tant que le bouton de droite n'est pas appuyer alors on reste sur l'écran de fin de jeu
  {
    // on va alors calcul les stat qui vont augmenté et celle qui vont descendre selon le temps en jeu
    if (minutesJeu >= 3)  // si je jeux a durer plus de 3 minutres
    {
      if (statOk == false)  // si les stat n'ont pas encore ete mise a jour alors on les mets a jour
      {
        truc.affection += 35;
        truc.fatigue += 30;
        truc.hunger += 25;
      }
      // puis on affiche les changement sur l'écran. et ce cera pareil pour les deux autres condition
      oled.setCursor(0, 18);
      oled.println("fatigue : + 30");

      oled.setCursor(0, 28);
      oled.println("faim : + 25");

      oled.setCursor(0, 38);
      oled.println("affection : + 35");
    }
    if (minutesJeu < 3 && minutesJeu >= 1)  // pareil
    {
      if (statOk == false) {
        truc.fatigue += 20;
        truc.hunger += 20;
        truc.affection += 20;
      }

      oled.setCursor(0, 18);
      oled.println("fatigue : + 20");

      oled.setCursor(0, 28);
      oled.println("faim : + 20");

      oled.setCursor(0, 38);
      oled.println("affection : + 20");
    } else  // pareil
    {

      if (statOk == false) {
        truc.fatigue += 10;
        truc.hunger += 15;
        truc.affection += 15;
      }

      oled.setCursor(0, 18);
      oled.println("fatigue : + 10");

      oled.setCursor(0, 28);
      oled.println("faim : + 15");

      oled.setCursor(0, 38);
      oled.println("affection : + 15");
    }

    // on affiche aussi le temps de jeu avec notre monstre
    oled.setCursor(0, 0);
    oled.println("Temps en jeu : ");
    oled.print(minutesJeu);
    oled.print(" min ");
    oled.print((int)secondesJeu);
    oled.println(" sec ");

    statOk = true;  // on mets a jour notre statOk car on a mis toute les stat a jour et ainsi eviter d'avjouter ou d'enleve des point de stats en boucle et seulement afficher les changement sur l'écran

    oled.setCursor(85, 50);
    oled.println("Quitter");  // on met un bouton quitter a gauche pour indiquer a l'utilisateur d'appuyer sur le bouton de gauche pour quitter.

    oled.display();
    delay(300);
  } while (digitalRead(boutonDroite) == 1);
}

void jeuDino() {
  oled.clearDisplay();   //effacer tout ce qui est l'écran
  oled.setCursor(4, 4);  // On plavce le curseur pour écrire au a l'emplacement 0, O
  oled.println(score);   // on écrit le score
  oled.setCursor(85, 4);
  oled.println("quitter");  // et en dessous le mot quitter

  if (secondesJeu > 60) {
    tempsJeuDebut = millis();
    minutesJeu += 1;
  }
  secondesJeu = (tempsJeuFinBoucle - tempsJeuDebut) / 1000;

  oled.drawBitmap(20, posYmonstre, squelette, 14, 15, WHITE);  // on dessine le monstre à l'enplacement x = 10 et y = posYmonstre = 64
  oled.drawBitmap(0, 16, fondJeu, 128, 49, WHITE);             // on met en place le fond d'écran du jeu
  oled.drawBitmap(posXnuage1, 16, nuage1, 20, 10, WHITE);      // on dessine troi fois les nuage a des emplacement différents et mais avec une auteur constante
  oled.drawBitmap(posXnuage2, 32, nuage1, 20, 10, WHITE);      // mais une une position X tel que x = posXnuage1 pour le 1 et etc pour les autres
  oled.drawBitmap(posXnuage3, 20, nuage1, 20, 10, WHITE);
  oled.drawLine(posXobstacle, 50, posXobstacle, 55, WHITE);
  oled.drawLine(posXobstacle1, 50, posXobstacle1, 55, WHITE);
  oled.drawLine(posXobstacle2, 50, posXobstacle2, 55, WHITE);
  obstacle();        //fonction qui permet de faire avancer l'obstacle sur l'écran
  nuage();           // fonction qui change les posXnuage de chacun afin de les faire bouger a chaque boucle
  sautPersonnage();  // fonction qui fait sauter le personnage en faisant bouger posYmonstre
  delay(25);
  tempsJeuFinBoucle = millis();
  oled.display();
}

void sautPersonnage() {
  if (digitalRead(boutonMilieu) == 0)  // si le bouton du milieu est activé alors on doit sauter donc on active le saut
  {
    doitSauter = true;
  }

  if (doitDescendre == true)  // s'implement que si le monstre doit descendre il ne peut pas sauter donc si le joueur réepui sur le bouton en descente et le monstre ne se bloque pas et poursuit sa descente.
  {
    doitSauter = false;
  }

  if (posYmonstre == (55 - 15) - 21)  // Si le monsre n'est pas sur terre alors il doit descendre donc true
  {
    doitDescendre = true;
  }

  if (posYmonstre == (55 - 15))  // Si le monstre est sur terre, il ne dois pas retombé donc false
  {
    doitDescendre == false;
  }


  if (doitSauter == true && doitDescendre == false)  // si le monstre est en saut et ne doit pas encore déscendre alors il poursuit son saut
  {
    posYmonstre -= 3;
  }
  if (doitSauter == false && doitDescendre == true && posYmonstre < (55 - 15))  // si le monstre ne saute plus alors il doit redescendre sans qu'il tarveerse le sol
  {
    posYmonstre += 3;
  }

  if (doitSauter == false && doitDescendre == true && posYmonstre == (55 - 15))  // si le monstre ne saute plus et qu'i est en cour de descsente mais attent le sol alors il ne dois plus descendre donc false
  {
    doitDescendre = false;
  }
}

void nuage() {
  if (posXnuage1 == -20)  // Si le nuage est sorti de l'écran soit sa posXnuage est sorti et ca pour les trois nuages
  {
    posXnuage1 = 128 + 20;  //s il retourne a son point de départ et ca pour les trois nuage a des position différente de départ
  }

  if (posXnuage2 == -20) {
    posXnuage2 = 128 + 20;
  }

  if (posXnuage3 == -20)

  {
    posXnuage3 = 128 + 20;
  }

  posXnuage1 -= 1;  // la on réduit la posXnuage de 1 pour les faire avancer sur l'écran en continu.
  posXnuage2 -= 1;
  posXnuage3 -= 1;
}

void obstacle() {
  if (posXobstacle == -1) {
    posXobstacle = 128 + 1;
  }

  if (posXobstacle1 == -1) {
    posXobstacle1 = 128 + 1;
  }

  if (posXobstacle2 == -1) {
    posXobstacle2 = 128 + 1;
  }

  posXobstacle -= 2;
  posXobstacle1 -= 2;
  posXobstacle2 -= 2;
}

// Fin des fonctions pour le jeu du Dino //

//manger//

void manger() {
  do {
    Serial.println(onglet);
    oled.clearDisplay();
    oled.drawLine(0, 14, 128, 14, WHITE);
    oled.drawBitmap(2, 42, gauche, 11, 11, WHITE);
    oled.drawBitmap(115, 42, droite, 11, 11, WHITE);
    if (digitalRead(boutonDroite) == 0) {
      if (onglet == 5) {
        onglet = 1;
      } else {
        onglet += 1;
      }
    }
    if (digitalRead(boutonGauche) == 0) {
      if (onglet == 1) {
        onglet = 5;
      } else {
        onglet -= 1;
      }
    }

    // Burger
    if (onglet == 1) {
      oled.drawBitmap(41, 17, burger, 46, 46, WHITE);
      oled.setCursor(2, 2);
      oled.print("Cheeseburger");
      oled.setCursor(80, 2);
      oled.print("+60 food");
    }
    // Brocoli
    if (onglet == 2) {
      oled.drawBitmap(43, 19, brocoli, 42, 41, WHITE);
      oled.setCursor(2, 2);
      oled.print("Broccoli");
      oled.setCursor(80, 2);
      oled.print("+30 food");
    }

    // Gateau
    if (onglet == 3) {
      oled.drawBitmap(43, 19, gateau, 42, 41, WHITE);
      oled.setCursor(2, 2);
      oled.print("Cake");
      oled.setCursor(80, 2);
      oled.print("+45 food");
    }
    //Cocktail
    if (onglet == 4) {
      oled.drawBitmap(43, 19, cocktail, 42, 41, WHITE);
      oled.setCursor(2, 2);
      oled.print("Cocktail");
      oled.setCursor(80, 2);
      oled.print("+15 food");
    }
    //Café
    if (onglet == 5) {
      oled.drawBitmap(44, 19, cafe, 42, 41, WHITE);
      oled.setCursor(2, 2);
      oled.print("Coffee");
      oled.setCursor(80, 2);
      oled.print("+15 food");
    }
    oled.display();
    delay(200);
  } while (digitalRead(boutonMilieu) == 1);
  // le reste
  if (onglet == 1) {
    truc.hunger -= 60;
  }
  if (onglet == 2) {
    truc.hunger -= 30;
  }
  if (onglet == 3) {
    truc.hunger -= 45;
  }
  if (onglet == 4) {
    truc.hunger -= 15;
  }
  if (onglet == 5) {
    truc.hunger -= 10;
  }
  onglet = 1;
}
