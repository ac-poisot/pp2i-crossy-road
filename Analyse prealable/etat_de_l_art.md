# État de l’art IA

## Approches :
### Approche 1 : Exploration exhaustive
Cet algorithme permet, pour chaque chemin visible d’optimiser à coup sûr le x atteint en une durée fixée si la durée fixée est inférieure à la distance entre le joueur et le haut de la fenêtre. La profondeur de recherche correspond alors à ladite durée.
Il admet tout de même un énorme défaut, sa complexité en fonction de la profondeur n est en O(5^n), le 5 correspondant au nombre de possibilités à chaque nœud de l’arbre de jeu.

### Approche 2 : Exploration avec élagage
On effectue alors une exploration exhaustive mais avec une condition d’arrêt de recherche sur un sous-arbre lorsque l’on sait que le résultat ne pourrait pas être supérieur au résultat actuel.
Dans notre cas, cela correspondrait à ne pas chercher les solutions proposées par une position à un temps donné si elles ont déjà calculées par une position explorée précédemment.
Cela permettrait alors d’éviter de calculer énormément de possibilités et la profondeur pourrait alors être augmentée significativement.
On pourrait tout de même être toujours confrontés à des problèmes de temps de calcul qui pourrait affecter la vitesse d’actualisation du jeu puisque l’IA tournerait dans le même processus

### Approche 3 : Réseau neuronal convolutif
Un réseau neuronal convolutif adapté à l’exploitation d’images et autres données en 2 dimensions et serait donc adapté au problème car la donnée à traiter peut simplement être la grille de m × n correspondant à la fenêtre visible à l’instant t.
Comme on chercherait à déterminer la meilleure option parmi les 5 possibilités à cet instant, on chercherait à effectuer une classification.

### Approche 4 : k-moyennes
En parlant de classification, on pourrait penser au seul algorithme de classification étudié pendant les études de MPI, l’algorithme des k-moyennes. Pourtant, cela n’est pas une bonne idée. Il faudrait définir une distance autre que celle euclidienne ou celle de Manhattan. Or, il nous semble compliqué de trouver une distance qui permettrait de classer les positions par meilleur mouvement à venir.

### Approche 5 : Algorithmes de graphes ?
Pour améliorer la recherche exhaustive, en donnant des indications sur un meilleur chemin (algorithme de la vague, Dijkstra, A*).
Dans notre cas, on peut considérer une arrivée et un départ comme les lanes sûres (forêts, voies ferrées quand le train ne passe pas). 
Cependant, cela signifie construire le graphe, bien que celui-ci puisse rester implicite, mais il faut quand même définir ce que sont les arêtes et les sommets (en l’occurrence, sommet = case et arête = transition, avec vers un obstacle +∞ et vers case vide la distance depuis le point de départ.)



## Sources:
- https://youtu.be/71UbDN4csas?t=591
- https://members.loria.fr/vthomas/mediation/JV_ESIAL_2013/2013_03_algo%20pour%20le%20jeu-IA_v23.pdf (p112)