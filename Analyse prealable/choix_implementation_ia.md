# Choix d'implémentation de l'IA

## Analyse du jeu :
Le jeu est composé de lignes d’obstacles, mouvants ou non, sur une dimension infinie ou quasi infinie comparée à jusqu’où un joueur humain est capable d’aller. On appellera dès lors cette dimension “x”.
Les mouvements des obstacles et du joueur sont régis par un temps discret.
A chaque instant, le joueur peut choisir entre 5 possibilités: se mouvoir dans l’une des 4 directions ou rester sur sa case.
La partie visible du jeu est une grille de m × n cases avec des objets se déplaçant ou non dessus.
L’objectif est d’amener le joueur le plus loin possible dans le jeu, ce qui correspond à aller au x le plus grand possible pour une fenêtre visible fixée et en une durée fixée.
On cherchera ici à développer une IA qui cherchera à maximiser le x atteint par le joueur qu’il contrôle. On pourra lui donner les mêmes données qu’un joueur humain aurait afin d’avoir une comparaison valable, c’est-à-dire la fenêtre visible à chaque instant.