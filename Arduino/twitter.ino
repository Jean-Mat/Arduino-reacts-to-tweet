//-- On include les librairies necessaires --
#include <SPI.h>
#include <Ethernet.h>

#define BRUMISATEUR  A0
#define LEDR A1
#define LEDG A2
#define LEDB A3
#define VENTILO A4

//-- Adresse MAC du schild Ethernet --
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x9C, 0x03 };
//-- On va utliser un serveur DHCP qui va nous attribuer une adresse IP automatique --
//-- Mais au cas où il ne peut pas, on affecte nous meme l'IP --
IPAddress ip(192, 168, 0, 177);

//-- Serveur à joindre pour récupérer le nombre de Tweets --
char server[] = "home.gourdet.net";

//-- On initialise le client HTTP --
EthernetClient client;

//-- Variables globales --
const unsigned long requestInterval = 20000;  //Interval entre 2 relevé de tweets
boolean requested;                   // whether you've made a request since connecting
unsigned long lastAttemptTime = 0;   // last time you connected to the server, in milliseconds

String currentLine = "";            // string to hold the text from server
String tweet = "";                  // string to hold the tweet
boolean readingTweet = false;       // if you're currently reading the tweet



//-- Fonction SETUP executé qu'une fois au demarrage --
void setup() {
  //-- On ouvre le port serie à une vitesse de 9600 bauds --
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  //-- Configuration des pins --
  pinMode(BRUMISATEUR, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(VENTILO, OUTPUT);
    
  digitalWrite(BRUMISATEUR, LOW);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
  digitalWrite(VENTILO, LOW);

  //-- On demarre la connexion Ethernet uniquement avec l'adresse MAC --
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Impossible de configurer Ethernet en DHCP");
    //-- On tente de demarrer la connexion Ethernet en forçant l'adresse IP --
    Ethernet.begin(mac, ip);
  }
  
  Serial.print("Adresse IP :");
  Serial.println(Ethernet.localIP());
  
  //-- On laisse passer 1 seconde pour que le schild Ethernet puisse se connecter --
  delay(1000);
  Serial.println("Connexion...");

 
  // connect to Twitter:
  connectToServer();

}

//-- Programme principal / boucle infinie --
void loop() {
  if (client.connected()) {
    if (client.available()) {
      //-- On lit le caractère qui arrivent --
      char inChar = client.read();
      //Serial.print(inChar);

      //-- On ajoute le nouveau caractère à une chaine globale --
      currentLine += inChar; 

      //-- si le caractère est un retour chariot --
      if (inChar == '\n') {
        //-- On commence une nouvelle ligne --
        currentLine = "";
      } 
      
      // if the current line ends with <text>, it will
      // be followed by the tweet:
      if ( currentLine.endsWith("<nb>")) {
        // tweet is beginning. Clear the tweet string:
        readingTweet = true; 
        tweet = "";
      }
      // if you're currently reading the bytes of a tweet,
      // add them to the tweet String:
      if (readingTweet) {
        if (inChar != '<') {
          if (inChar != '>') {
            tweet += inChar;
          }
        } 
        else {
          // if you got a "<" character,
          // you've reached the end of the tweet:
          readingTweet = false;
          int nbTweet = tweet.toInt();
          Serial.print("nb tweets : ");
          Serial.println(nbTweet); 
          // close the connection to the server:
          client.stop();
          if(nbTweet > 0) { 
            action();
          }
        }
      }
    }   
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    connectToServer();
  }
}

//-- Fonction qui se connecte au serveur pour recupérer le nombre de tweet --
void connectToServer() {
  // attempt to connect, and wait a millisecond:
  Serial.println("Connexion au serveur...");
  if (client.connect(server, 80)) {
    Serial.println("On construit la requete HTTP...");
    // make HTTP GET request to twitter:
    client.println("GET /zBis/index.php HTTP/1.1");
    client.println("HOST: home.gourdet.net");
    client.println("Connexion: close");
    client.println();
  } else {
    Serial.println("Echec de connexion");
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}

void action() {
  Serial.println("action");
  
    //-- On allume le brumisateur --
    digitalWrite(BRUMISATEUR, HIGH);
    
    //-- 1 seconde de led rouge --
    digitalWrite(LEDR, HIGH);
    delay(1000);
    digitalWrite(LEDR, LOW);
    
    //-- 1 seconde de led verte --
    digitalWrite(LEDG, HIGH);
    delay(1000);
    digitalWrite(LEDG, LOW);    

    //-- 1 seconde de led bleu --
    digitalWrite(LEDB, HIGH);
    delay(1000);
    
    //-- 5 secondes toutes les couleurs --
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);
    delay(8000);
    
    //-- On etaint tout --
    digitalWrite(BRUMISATEUR, LOW);
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, LOW);
    
    //-- Un petit coups de ventilo --
    digitalWrite(VENTILO, HIGH);
    delay(500);
    digitalWrite(VENTILO, LOW);
}
