# Etat de l’Art de l’IA dans les jeux de stratégie


## Table des matières
[Introduction](#introduction)
[Le rôle de l’intelligence artificielle dans les jeux vidéo](#le-rôle-de-lintelligence-artificielle-dans-les-jeux-vidéo)
[Les intelligences artificielles existantes dans les jeux](#les-intelligences-artificielles-existantes-dans-les-jeux)
[Sources](#sources)


## Introduction

Le développement des Intelligences Artificielles (IA) est de plus en plus mis en avant. Cela modifie notre environnement en nous proposant de plus en plus de services pour nous aider au quotidien, comme générer du texte avec ChatGPT ou du code avec GitHub Copilot. On définit l’IA comme étant un ensemble de techniques et de théories dont l’objectif est d’émuler ou de simuler les comportements humains sans toutefois s’y limiter. Le terme est utilisé pour la première fois en 1956 par John McCarthy lors de la conférence de Dartmouth. McCarthy travaillera ensuite avec Marvin Minsky au MIT pour continuer à la développer.

On confond souvent intelligence artificielle avec apprentissage supervisé ou encore réseaux neuronaux. L’apprentissage supervisé consiste à faire apprendre en donnant en entrées de grands jeux de données et les étiquettes associées représentant les sorties attendues. Les réseaux neuronaux consistent en des éléments, appelés perceptrons, reliés entre eux et appliquant des fonctions mathématiques sur leurs entrées et leurs poids pour calculer leurs sorties, la sortie du réseau pouvant être de différentes sortes, que ce soit binaire, flottant ou une classe. On entraîne alors généralement ces réseaux, appelés modèles sur des jeux de données (datasets) afin de modifier les poids et obtenir le résultat le plus précis possible, même si d’autres méthodes telles qu’avec des algorithmes génétiques sont possibles.

Les IA ont été intégrées aux jeux très tôt, dans des jeux comme Pong, Space Invaders, Pac-Man et Rogue. En effet, elles permettent de simuler les comportements des autres joueurs et d’obtenir une expérience plus immersive.



## Le rôle de l’intelligence artificielle dans les jeux vidéo

La première utilisation de l’IA est d’offrir des ennemis intelligents. Cela peut être effectué dans de nombreux jeux avec diverses IA plus ou moins développées. En effet, une IA permet de proposer une expérience de jeu plus intéressante qu’avec simplement des actions aléatoires. Cela permet notamment de l’humaniser.
Les IA permettent également de générer un monde complexe et cohérent. En effet, dans des jeux d’exploration, le joueur interagit avec l’environnement du jeu : il parle aux personnages non joueurs (PNJ), prend des décisions comme mettre le feu, empoisonner l’eau, déplacer des objets… Pour avoir une bonne expérience de jeu, il faut donc que celui ci-ci réagisse en fonction de ces actions : un personnage qui se souviendrait que le joueur lui a volé quelque chose sera plus hostile qu’un personnage qu’on a aidé à chercher un objet, ou dans le cas des jeux de gestion, tous les éléments interagissent entre eux. Il faut donc savoir ce qui se passe dans une zone A si le joueur est dans un zone B. On peut rajouter dans cette catégorie le fait de s’adapter au joueur, si celui-ci fait le même type de choix (pierre-feuille-ciseaux). De même, cela permet de créer les animaux qui s’adaptent au joueur.



## Les intelligences artificielles existantes dans les jeux
	
L’algorithme le plus simple est généralement l’exploration exhaustive. Elle consiste en le calcul de toutes les possibilités à partir de l’état actuel de la partie afin de déterminer quelle est la stratégie à adopter pour optimiser ses chances de gagner. C’est un algorithme qui permet la plupart du temps d’obtenir un très bon résultat mais qui a une complexité temporelle importante ce qui empêche souvent son usage, notamment dans les jeux commerciaux. Il s’applique tout de même à de nombreux petits jeux dans des espaces finis tels que le TicTacToe ou le jeu de Nimm.
De nombreuses variantes existent pour différents types de jeu tel que l’algorithme Min-Max, par exemple, dans le cadre d’un jeu à 2 joueurs à somme nulle, qui lui-même admet des variantes comme l’élagage alpha-bêta. L’algorithme Min-Max consiste à parcourir l’arbre des configurations, c’est-à-dire l’ensemble des configurations atteignables en jouant chacun son tour. Il part ensuite des feuilles en maximisant le score si c’est le joueur qui fait jouer et en minimisant le score si c’est l’autre joueur. L’objectif étant d’obtenir le score le plus grand pour gagner ou minimiser une défaite. Dans la variante de l’élagage alpha-beta, on évite de parcourir toutes les possibilités quand on sait qu’on ne peut pas avoir un score plus petit ou plus grand.

Cependant, de tels algorithmes ne sont pas applicables à énormément de jeux puisqu’ils ne fonctionnent que sur des jeux avec des règles simples et un monde peu complexe. Le jeu d’échecs suffit à démontrer la difficulté d’utiliser de telles IAs puisqu’il y a 10^120 parties possibles avec seulement 32 pièces et un plateau de 8 cases de large. Que dire donc de jeux tels que League of Legends ou StarCraft II ?
Une solution souvent utilisée lorsque la complexité devient trop complexe est l’usage de réseaux neuronaux. Les résultats peuvent alors être époustouflants tels que DeepBlue en 1997 face au champion du monde d’échecs Kasparov ou 20 ans plus tard, Alphago qui vainquit Ke Jie.
Ces IA sont alors construites à l’aide de l’apprentissage,en se basant sur le modèle BDI et de l’apprentissage renforcé et des réseaux de neurones, avec des valeurs associées à des couples (Situation->Action). Le modèle BDI (Belief-Desire-Intention) possède trois éléments : Croyance, Désir et Intention. Les croyances sont l’ensemble des informations que possède l’IA. Elles peuvent être vraies, erronées ou partielles, mais sont toujours considérées comme vraies. Les désirs sont les configurations que l’IA recherche. Il peut y en avoir plusieurs, éventuellement contradictoires, ce qui implique un choix en fonction des croyances. Les intentions sont les désirs ou les actions choisies pour atteindre les désirs. Il permet d’avoir un raisonnement “humain”, notamment dans le fait d’être pessimiste ou optimiste (garder des objets trouvés pour possiblement s’en resservir). On peut ensuite le modéliser à l’aide d’un graphe. Pour passer à l’apprentissage, on ajoute les résultats des actions à un historique qui vient influer sur le modèle BDI. Cependant, il est bon de remarquer que l’apprentissage prend beaucoup de temps, car il faut créer l’historique.
Les réseaux neuronaux consistent en des éléments, appelés perceptrons, reliés entre eux et appliquant des fonctions mathématiques sur leurs entrées et leurs poids pour calculer leurs sorties, la sortie du réseau pouvant être de différentes sortes, que ce soit binaire, flottant ou une classe. On entraîne alors généralement ces réseaux, appelés modèles sur des jeux de données (datasets) afin de modifier les poids et obtenir le résultat le plus précis possible, même si d’autres méthodes telles qu’avec des algorithmes génétiques sont possibles.
	
Une autre solution consiste à s’appuyer sur l’aléatoire. Par exemple, la Recherche arborescente Monte-Carlo se base sur la méthode de Monte-Carlo qui permet de déterminer approximativement une valeur à partir de données et d'aléatoire. La RaMC calcule donc l'action la plus probable à appliquer mais a une chance de tester des branches dites prometteuses.


## Sources
https://fr.wikipedia.org/wiki/Intelligence_artificielle
https://fr.wikipedia.org/wiki/Histoire_de_l%27intelligence_artificielle
https://fr.wikipedia.org/wiki/Intelligence_artificielle_dans_le_jeu_vid%C3%A9o
https://members.loria.fr/vthomas/mediation/JV_ESIAL_2013/2013_03_algo%20pour%20le%20jeu-IA_v23.pdf
https://fr.wikipedia.org/wiki/Mod%C3%A8le_logiciel_de_croyance%E2%80%93d%C3%A9sir%E2%80%93intention
https://turing.cs.pub.ro/auf2/html/chapters/chapter2/chapter_2_2_2.html
https://hal.science/hal-00742874/document (beaucoup plus théorique)

