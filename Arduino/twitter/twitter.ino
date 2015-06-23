//-- On include les librairies necessaires --
#include <SPI.h>
#include <Ethernet.h>

#define BRUMISATEUR A0
#define RUBAN_LED   A1
#define VENTILO     A2
#define LED_OK      A3
#define LED_KO      A4
#define LED_ACTION  A5

//-- Adresse MAC du schild Ethernet --
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x9C, 0x03 };
//-- On va utliser un serveur DHCP qui va nous attribuer une adresse IP automatiquement --
//-- Mais au cas où il ne peut pas, on affecte nous meme l'IP --
IPAddress ip(192, 168, 0, 177);

//-- Serveur à joindre pour récupérer le nombre de Tweets --
char server[] = "www.domaine.com";

//-- On initialise le client HTTP --
EthernetClient client;

//-- Variables globales --
const unsigned long requestInterval = 5000;  //Interval entre 2 relevé de tweets
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
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_KO, OUTPUT);
  pinMode(LED_ACTION, OUTPUT);
  
  pinMode(BRUMISATEUR, OUTPUT);
  pinMode(RUBAN_LED, OUTPUT);
  pinMode(VENTILO, OUTPUT);
    
  digitalWrite(LED_OK, LOW);
  digitalWrite(LED_KO, LOW);
  digitalWrite(LED_ACTION, LOW);
  
  digitalWrite(BRUMISATEUR, LOW);
  digitalWrite(RUBAN_LED, LOW);
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
      
      //Serial.println(currentLine);
      
      // if the current line ends with <text>, it will
      // be followed by the tweet:
      if ( currentLine.endsWith("<nbTweet>")) {
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
            for(int i = 0 ; i < nbTweet ; i++) {
              action();
              delay(1000);
            }
          }
        }
      }
      
    }   
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    connectToServer();
  }
}

//-- Fonction qui se connecte au serveur pour recupérer le nombre de tweet --
void connectToServer() {
  // attempt to connect, and wait a millisecond:
  Serial.println("Connexion au serveur...");
  if (client.connect(server, 80)) {
    cnx(true);
    Serial.println("On construit la requete HTTP...");
    // make HTTP GET request to twitter:
    client.println("GET /getTweet.php HTTP/1.1");
    client.println("HOST: www.domaine.com");
    client.println("Connexion: close");
    client.println();
  } else {
    Serial.println("Echec de connexion");
    cnx(false);
    
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}

void action() {
    Serial.println("action");
    digitalWrite(LED_ACTION, HIGH);
  
    //-- On allume le brumisateur --
    digitalWrite(BRUMISATEUR, HIGH);
    
    //-- On allume le ruban led --
    digitalWrite(RUBAN_LED, HIGH);
    
    //-- On laisse la magie operee 10 secondes --
    delay(10000);
    
    //-- On eteint le brumisateur --
    digitalWrite(BRUMISATEUR, LOW);
    
    //-- Un petit coups de ventilo --
    digitalWrite(VENTILO, HIGH);
    delay(1000);
    digitalWrite(VENTILO, LOW);
        
    
    //-- On eteint le ruban led --
    digitalWrite(RUBAN_LED, LOW);
      
    digitalWrite(LED_ACTION, LOW);
}

void cnx(bool ok) {
  if(ok) {
    digitalWrite(LED_OK, HIGH);
    digitalWrite(LED_KO, LOW);
  } else {
    digitalWrite(LED_OK, LOW);
    digitalWrite(LED_KO, HIGH);
  }
}
