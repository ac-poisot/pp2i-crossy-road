# Introduction
Le projet est séparé en plusieurs partie, le CORE, l'IA, le CLI, et le GUI :
- Le __CORE__ est le noyau du jeu, toutes les fonctions de calculs majeurs des états du jeu sont réunies dans celui-ci.
- L'__IA__ et le fichier minmax contiennent les fonctions de calculs des possibilité de notre modèle d'IA.
- Le __CLI__ permet d'avoir une représentation visuelle du jeu sur shell, pour développer les fonctionnalité finales. Il contient une boucle de jeu, peut être lancée avec la commande `make run_cli`. Il n'a pas de fichiers test.
- Le __GUI__ est le fichier finale utilisant la bibliothèque SDL pour avoir un exécutable du jeu. Il peut lui aussi être lancée avec la commande `make run_graphics` et n'a pas non plus de fichier test.

# CORE

| Biomes  | Valeurs Associées |
| ------- | ----------------- |
| Forêt   | 1                 |
| Rivière | 2                 |
| Rail    | 3                 |
| Route   | 4                 |

| Pouvoir   | Valeurs associées | EFFET                                                  |
| --------- | ----------------- | ------------------------------------------------------ |
| XIV       | 1                 | Permet de marcher sur l'eau                            |
| NEO       | 2                 | Ralenti le temps temporairement                        |
| TANK      | 3                 | Permet d'écraser les trains, voitures et arbres        |
| ECOLO     | 4                 | Transforme la zones environnantes en jolie plaine      |
| CRESUS    | 5                 | Multiplie temporairement le gains de pièces $\times$ 3 |
| POWER_END | 6                 | Variable de fin de pouvoir                             |
## Structures
| obstacle  |      |
| --------- | ---- |
| obstacle* | next |
| obstacle* | prev |
| float     | x    |
| int       | size |

| float_list  |       |
| ----------- | ----- |
| float       | val   |
| int         | power |
| float_list* | next  |

| lanes       |           |
| ----------- | --------- |
| int         | y         |
| float       | speed     |
| obstacle*   | obstacles |
| int         | obst_size |
| float_list* | coins     |
| int         | type      |
| lane*       | prev      |
| lane*       | next      |

| player    |             |
| --------- | ----------- |
| float     | y           |
| float     | x           |
| int       | orientation |
| int       | skin        |
| bool      | dead        |
| int       | power       |
| int       | power_time  |
| int       | anim        |
| int       | buffer      |
| obstacle* | on_log      |
| float     | liftboost   |
| float     | x_offset    |

| displayedData |                   |
| ------------- | ----------------- |
| float         | cameraY           |
| lane*         | first_lane        |
| lane*         | camera_first_lane |
| player        | player            |
| player        | ai                |
| int           | score             |
| int           | gameOver          |

## Fonctions

### void free_obstacles(obstacle * first_obstacle)
La fonction prend une liste d'*obstacle* en entrée est libère toute la liste.
### void free_coins(float_list * first_coin)

La fonction prend une liste de *coins* en entrée est la libère.
### void free_lanes(lane * first_lane)
La fonction prend une *lane* en entrée et va récursivement libéré la *lane* , les listes d'*obstacles* et de *coins* que les *lanes* contiennent.

### int biome(int * boules)
La fonction prend une liste de 4 entiers, représentant le poids des 4 biomes en entrée et retourne un biome aléatoirement parmi les 4 .
### int * probabilite_biomes(lane * l)
La fonction prend une liste de 4 *lanes* et calcule le poids des 4 biomes pour la prochaine *lanes* à générer.

### lane * empty_lane(lane * prev_lane, int type)
La fonction prend une *lane*, un biome et rattache la nouvelle *lane* vide a la *lane* initiale.

### lane * initialLanes(void)
La fonction créer les 4 premières *lanes* du jeu, qui sont toutes du biomes forêts.

### lane * random_lane(lane * prev_lane)
Créer aléatoirement une *lane* remplit, elle ne sert que pour des test.

### bool * create_obstacles_array(obstacle * o)
Créer une liste de *booléen* représentant la présence ou non d'un obstacle sur une ligne.

### void array_not(bool * array)
Applique `not` sur une liste de *booléen* 
### bool * array_and(bool * a, bool * b)
Applique `and` entre 2 liste de *booléen*
### bool array_exist(bool * array)
Renvoie true si au moins un *booléen* est true
### bool reachable(lane * l, bool * a, float speed)

### obstacle * generate_vehicles(lane * l)
La fonction ajoute des voitures à la *lane*
### obstacle * generate_trees(lane * l)
La fonction ajoute des arbre à la *lane*
### obstacle * generate_waterlilies(lane * l)
La fonction ajoute des nénuphar à la *lane*
### obstacle * generate_drowning_slots(void)

### obstacle * generate_trains(void)
La fonction ajoute des trains à la *lane*
### lane * generate_lane(lane * prev_lane, int type)

### void update_vehicles(lane * l)
La fonction met à jour les véhicules de la *lane*
### void update_drowning_slots(lane*  l)
La fonction met à jour les troncs de la *lane*
### void update_trains(lane * l)
La fonction met à jour les trains de la *lane*
### void display_obstacles(obstacle* l)
Fonction de debug, affiche la position des véhicules
### void displayLanes(lane * l)
Fonction de debug, affiche l'état du jeu à un instant t sur le terminal 
### void updateLanes(lane * l)
La fonction appel met à jour toute les *lanes* de la listeune par une
### void updateLane(lane * l)
La fonction met à jour la *lane* donnée
### obstacle * collides(lane * current_lane, player player)
La fonction renvoie un *obstacle* si le personnage entre en contact avec un *obstacle*
### float_list* collides_coin(lane *current_lane, displayedData game)
La fonction renvoie un *coin* si le personnage entre en contact avec un *coin*
### displayedData move_camera(displayedData data, float speed)
La fonction déplace la caméra d'une *lane* en avant, et re-créer une nouvelle *lane*
### displayedData init_game(int game_height)
La fonction initialise le jeu
### displayedData power4 (displayedData data)
La fonction applique le pouvoir ECOLO au jeu, transformant les 3 lignes les plus proches en *lane* forêt vide
### displayedData tank_road (displayedData data,obstacle* collided_obstacle, lane* current_lane)
La fonction supprime la voiture ou l'arbre en collision avec le personnage
### displayedData tank_train (displayedData data)
La fonction supprime le train en collision avec le personnage
# IA
__Intelligence Artificielle__
# CLI
__Commande 
# GUI