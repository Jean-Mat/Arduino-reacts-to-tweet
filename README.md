# Arduino-reacts-to-tweet

Permet de de faire réagir un objet aux tweets avec une carte Arduino et un Shield Ethernet.

Le code arduino twitter.ino se connecte au serveur qui héberge un script PHP (getTweet.php)
En fonction du retour de ce script, la carte arduino déclenche une action ou non sur les sorties.

En l'occurence, dans cet éxemple il s'agit d'une composition florale qui réagit aux tweets sur un HASHTAG précis.
Toutes les 5 secondes, la carte arduino interroge le script. Si 1 nouveau tweet est disponible, alors on déclenche des leds et un brumisateur pour animé cette composition florale.
Sinon, on ne fait rien.
