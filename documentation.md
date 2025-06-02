# Introduction
Le projet est séparé en plusieurs parties, le CORE, l'IA, le CLI, et le GUI :
- Le __CORE__ est le noyau du jeu, toutes les fonctions de calculs majeurs des états du jeu sont réunies dans celui-ci.
- L'__IA__ et le fichier ``minmax.c`` contiennent les fonctions de calculs des possibilité de notre modèle d'IA.
- Le __CLI__ permet d'avoir une représentation visuelle du jeu sur shell, pour développer les fonctionnalités finales. Il contient une boucle de jeu, peut être lancée avec la commande `make run_cli`. Il n'a pas de fichier test.
- Le __GUI__ est le fichier final utilisant la bibliothèque SDL pour avoir un exécutable du jeu. Il peut lui aussi être lancé avec la commande `make run_graphics` et n'a pas non plus de fichier test.

# CORE

| Biomes  | Valeurs Associées |
| ------- | ----------------- |
| Forêt   | 1                 |
| Rivière | 2                 |
| Rails   | 3                 |
| Route   | 4                 |

| Pouvoir   | Valeurs associées | EFFET                                                  |
| --------- | ----------------- | ------------------------------------------------------ |
| XIV       | 1                 | Permet de marcher sur l'eau                            |
| NEO       | 2                 | Ralentit le temps temporairement                        |
| TANK      | 3                 | Permet d'écraser les trains, voitures et arbres        |
| ECOLO     | 4                 | Transforme la zones environnante en jolie plaine      |
| CRESUS    | 5                 | Multiplie temporairement le gain de pièces $\times$ 3 |

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
- La fonction prend une liste d'*obstacle* en entrée et libère toute la liste.
### void free_coins(float_list * first_coin)
- La fonction prend une liste de *coins* en entrée et la libère.
### void free_lanes(lane * first_lane)
- La fonction prend une *lane* en entrée et va récursivement libérer la *lane* , les listes d'*obstacles* et de pièces/pouvoirs que les *lanes* contiennent.

### int biome(int * boules)
- La fonction prend une liste de 4 entiers, représentant le poids des 4 biomes en entrée et retourne un biome aléatoirement parmi les 4.
### int * probabilite_biomes(lane * l)
- La fonction prend une liste de 4 *lanes* et calcule le poids des 4 biomes pour la prochaine *lanes* à générer.

### lane * empty_lane(lane * prev_lane, int type)
- La fonction prend une *lane*, un biome et rattache la nouvelle *lane* vide à la *lane* initiale.

### lane * initialLanes(void)
- La fonction crée les 4 premières *lanes* du jeu, qui sont toutes du biomes forêts.

### lane * random_lane(lane * prev_lane)
- Crée aléatoirement une *lane* remplie, sert principalement aux tests.

### bool * create_obstacles_array(obstacle * o)
- Crée un tableau de booléans représentant la présence ou non d'un obstacle sur une ligne.

### void array_not(bool * array)
- Applique l'opérateur `not` sur un tableau de booléens.
### bool * array_and(bool * a, bool * b)
- Applique l'opérateur `and` entre deux tableaux de booléens.
### bool array_exist(bool * array)
- Renvoie true si au moins un booléen du tableau est vrai.
### bool reachable(lane * l, bool * a, float speed)
- 
### obstacle * generate_vehicles(lane * l)
- La fonction ajoute des voitures à la *lane*.
### obstacle * generate_trees(lane * l)
- La fonction ajoute des arbres à la *lane*.
### obstacle * generate_waterlilies(lane * l)
- La fonction ajoute des nénuphars à la *lane*.
### obstacle * generate_logs(void)
- La fonction ajoute des rondins à la *lane*.
### obstacle * generate_trains(void)
- La fonction ajoute des trains à la *lane*.
### lane * generate_lane(lane * prev_lane, int type)
- La fonction crée une nouvelle *lane* de type précisé et la raccroche à celle donnée en paramètre.
### void update_vehicles(lane * l)
- La fonction met à jour les véhicules de la *lane*.
### void update_logs(lane*  l)
- La fonction met à jour les troncs de la *lane*.
### void update_trains(lane * l)
- La fonction met à jour les trains de la *lane*.
### void display_obstacles(obstacle* l)
- Fonction de debug, affiche la position des véhicules.
### void displayLanes(lane * l)
- Fonction de debug, affiche l'état du jeu à un instant t sur le terminal.
### void updateLanes(lane * l)
- La fonction met à jour toute les *lanes* une par une à partir de celle donnée en entrée.
### void updateLane(lane * l)
- La fonction met à jour la *lane* donnée.
### obstacle * collides(lane * current_lane, player player)
- La fonction renvoie un ``obstacle`` si le personnage entre en contact avec cet ``obstacle``, ``NULL`` sinon.
### float_list* collides_coin(lane *current_lane, displayedData game)
- La fonction renvoie un la pièce/pouvoir si le personnage entre en contact avec celui-ci, ``NULL``sinon.
### displayedData move_camera(displayedData data, float speed)
- La fonction déplace la caméra en avant, et re-crée une nouvelle *lane* si nécessaire.
### displayedData init_game(int game_height)
- La fonction initialise le jeu;
### displayedData power4 (displayedData data)
- La fonction applique le pouvoir ECOLO au jeu, transformant les 3 lignes les plus proches en *lane* forêts vides.
### displayedData tank_road (displayedData data,obstacle* collided_obstacle, lane* current_lane)
- La fonction supprime la voiture ou l'arbre en collision avec le personnage.
### displayedData tank_train (displayedData data)
- La fonction supprime le train en collision avec le personnage.
# I.A.
__Intelligence Artificielle__

Le fichier minmax contient des fonctions pour faire des copies profondes pour pouvoir faire des calculs sans modifier le jeu en cours.

### copy_obstacles(obstacle* o)
- Réalise une copie profonde des obstacles
### copy_lane(lane* l, int p) 
- Réalise une copie profonde des p lanes devant et derrière le joueur. Le résultat est la lane du début, donc le début des lanes copiées doit être obtenu allieurs pour libérer la mémoire.
### collides_without_game(lane* current_lane, player p)
- Vérifie que le joueur n'entre pas en collision avec un obstacle
### update_lanes(lane* l)
- Met à jour les lanes depuis la lane passée en paramètre
### minmax_rec(lane* l, int deep, couple previous, player p)
- Calcule le meilleur mouvement possible grâce à un minmax. Renvoie dans une structure spéciale couple. Si on obtient un score égal au score meilleur actuel, on garde celui obtenu précédement.
### minmax_simple(lane* l, int deep, player p)
- Réalise un appel à ``minmax_rec`` et ne renvoie que le mouvement à faire.
### n_update(int n, lane* l)
- Met dans un tableau les n mises à jour futures pour éviter des les recalculer. En case i, il y a la mise à jour n-i-1.
### free_update(lane** tab, int n)
- Libère la mémoire associée aux mises à jour.
### minmax_rec_memo_state(int deep, couple previous, player p, lane** tab)
- Même chose que minmax_rec mais avec un tableau des updates pour éviter des calculs.
### minmax_memo_state(lane* l, int deep, player p)
- Réalise un appel à ``minmax_rec_memo_state`` et ne renvoie que le mouvement à faire.
### play_ai(int cai, lane* player_lane, player ai)
- Permet de jouer de façon générique avec l'I.A. dans CLI et GUI.

# CLI
__Command Line Interface__

### void init_colors(void);
- Initialise les couples de couleurs ncurses.

### void display_lane(lane* lane, int lane_count);
- Permet d'afficher une *lane* et tout ce qu'elle comprend dans le terminal.

### void display(displayedData data);
- Permet d'afficher dans le terminal l'ensemble des informations liées au jeu.

### void display_title_animation(void) ;
- Permet d'afficher et de gérer l'animation liée à l'écran titre.

### void display_shop(void);
- Permet d'afficher l'écran du magasin de *skins*.

### int main(void);
- La fonction principale qui permet de lancer le jeu en ligne de commande.

# GUI
__Graphics User Interface__

## Structure
| button  |      |
| --------- | ---- |
| int | x |
| int | y |
| int     | width    |
| int       | height |
| int       | texture |

## Fonctions ``button.c``

### void display_button(button b, SDL_Renderer* renderer, SDL_Texture** textures)
- Permet d'afficher le bouton spécifié en paramètre dans le *renderer* SDL.

### bool button_clicked(button b, SDL_Event event);
- Renvoie si le curseur de la souris est sur le bouton et si le clic gauche est appuyé.

## Fonctions ``display.c``

### void display_text(char* text, int x, int y, int size, SDL_Renderer* renderer, SDL_Color color, char* font, bool bg);
- Permet d'afficher du texte d'une couleur, taille, et police d'écriture spécifiée. ``bg`` permet d'y ajouter un fond noir pour rendre le texte plus lisible.

### void display_coins(float y, float_list* current_coin, SDL_Renderer* renderer, SDL_Texture** textures);
- Permet d'afficher toutes les pièces/pouvoirs présents sur une ligne.

### void display_lane(lane* lane, float lane_count, SDL_Renderer* renderer, SDL_Texture** textures);
- Permet d'afficher toute une *lane* et ce qu'il y a dessus.

### void displayPlayer(player player, float cameraY, SDL_Renderer* renderer, SDL_Texture** textures, int skin_set);
- Permet d'afficher un joueur.

### void display(displayedData data, SDL_Renderer* renderer, SDL_Texture** textures, int skin_set, int power_duration);
- Permet d'afficher toutes les informations liées à une partie : les *lanes*, le(s) joueur(s)...

## Fonctions ``sound_management.h``

### void load_sounds(Mix_Chunk** sounds, Mix_Music** music);
- Permet de charger tous les sons et musiques du jeu.
### void free_sounds(Mix_Chunk** sounds, Mix_Music** music);
- Permet de libérer tous les sons et musiques du jeu.

## Fonctions ``sprite_management.h``

### SDL_Texture* create_texture(SDL_Renderer* renderer, char* filename, int width, int height);
- Permet à partir d'un nom de fichier, de créer une texture SDL.

### void load_textures(SDL_Renderer* renderer, SDL_Texture** textures, char** skin_names, int sprite_set);
- Permet de charger l'entièreté des textures du jeu.

### void free_textures(SDL_Texture** textures);
- Permet de libérer toutes les textures du jeu.

### void free_skin_names(char** skin_names);
- Permet de libérer les chaines de caractères associées aux noms des différents *skins*.

## Fonctions ``gui.c``

### void reset_savefile(void)
- Fonction permettant de réinitialiser le fichier ``data.txt``.
### void update_savefile(int high_score, int purse, bool* unlocked_skins)
- Permet de sauvegarder les multiples variables extérieures au jeu entre les parties, dans le fichier ``data.txt``.
### int process_player(player* current_p, displayedData* game, Mix_Chunk** sounds, int* purse, bool* buffer_key_flag, bool is_ai)
- Réalise tous les tests de collisions pour un joueur et met à jour toutes les variables associées. Renvoie si le joueur est mort ou non.
### int main(void)
- La fonction principale qui permet de lancer le jeu en fenêtre graphique.