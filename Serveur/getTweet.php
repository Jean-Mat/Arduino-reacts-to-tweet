<?php
	//-- On précise au serveur qu'on ne veut pas de cache sur ce fichier --
	header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
	header("Expires: Sat, 26 Jul 1997 05:00:00 GMT"); // Date dans le passé

	ini_set('display_errors', 1);

	//-- On charge la configuration et la librairie TwitterAPI --
	require_once('config.php');
	require_once('TwitterAPIExchange.php');

	//-- URL pour récupérer les tweets --
	$url = 'https://api.twitter.com/1.1/search/tweets.json';

	//-- Paramètre pour précisier quel hashtag on veux rechercher --
	$params = '?q='.urlencode(str_replace(', ', ' ', HASHTAG));
	//-- On récupère l'ID du dernier tweet récupéré --
	$since = file_get_contents("tweet.txt");
	//-- Si on a l'ID du dernier tweet récupéré, on le passe en paramètre pour récupérer les tweet DEPUIS celui-ci --
	if(isset($since)) $params.='&since_id='.$since;

	//-- On se connecte à l'API Twitter --
	$twitter = new TwitterAPIExchange();
	//-- On execute la commande pour récupérer les tweets --
	$retour = $twitter	->setGetfield($params)
				 		->buildOauth($url, 'GET')
				 		->performRequest();

	$retour = json_decode($retour);

	//-- On récupère la liste des tweets --
	if (isset($retour->statuses)) {
      $tweets = $retour->statuses;
    }

    //-- On compte le nombre de tweets récupérés --
    $nbTweet = count($tweets);

    //-- Si on a récupéré des tweets, on stock l'ID du dernier pour la prochaine fois --
    if($nbTweet > 0) {
    	$lastTweetId = $tweets[0]->id_str;
    	file_put_contents("tweet.txt", $lastTweetId);
    }

    //-- On envoie en sortie un valeur compatible avec Arduino --
    echo '<nbTweet>'.$nbTweet.'</nbTweet>';

?>
