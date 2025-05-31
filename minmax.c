#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "minmax.h"
#include "list.h"

#define LANE_WIDTH 24 // width of the displayed area
#define UNPLAYABLE_WIDTH 2

#define NOT_POSSIBLE 0
#define GO_AHEAD 1
#define GO_DOWN  2
#define GO_RIGHT 3
#define GO_LEFT  4
#define STAY     5

obstacle* copy_obstacle(obstacle* o) {
    // deep copy of the obstacles
    if (o == NULL) {
        return NULL;
    } else {
        obstacle* copy = (obstacle*)malloc(sizeof(obstacle));
        copy->x = o->x;
        copy->size = o->size;
        copy->next = NULL;
        copy->prev = NULL;
        obstacle* last_copy = copy;
        obstacle* o_cp = o->next;
        while (o_cp != NULL) {
            obstacle* new = (obstacle*)malloc(sizeof(obstacle));
            new->x = o_cp->x;
            new->size = o_cp->size;
            new->next = NULL;
            new->prev = last_copy;
            last_copy->next = new;
            last_copy = new;
            o_cp = o_cp->next;
        }
        return copy;
    }
}

lane* copy_lane(lane* l, int p) {
    // deep copy of the p lanes in front of and behind the player but without coins
    // the result is the current lane copied, so the 'real' first lane must be got to be freed
    if (l == NULL) {
        return NULL;
    } else {
        lane* copy = (lane*)malloc(sizeof(lane));
        copy->y = l->y;
        copy->speed = l->speed;
        copy->obst_size = l->obst_size;
        copy->type = l->type;
        copy->prev = NULL;
        copy->next = NULL;
        copy->obstacles = copy_obstacle(l->obstacles);
        copy->coins = NULL; //NULL for the moment, possible futur bug
        // copies the next p lanes or stops before
        lane* sauv = l->next;
        lane* last_copied = copy;
        int k = p;
        while (k > 0 && sauv != NULL) {
            lane* new_lane = (lane*)malloc(sizeof(lane));
            new_lane->y = sauv->y;
            new_lane->speed = sauv->speed;
            new_lane->obst_size = sauv->obst_size;
            new_lane->type = sauv->type;
            new_lane->next = NULL;
            new_lane->obstacles = copy_obstacle(sauv->obstacles);
            new_lane->coins = NULL; //NULL for the moment, possible futur bug
            new_lane->prev = last_copied;
            last_copied->next = new_lane;
            last_copied = new_lane;
            sauv = sauv->next;
            k--;
        }
        // copies the previous p lanes or stops befores
        sauv = l->prev;
        last_copied = copy;
        k = p;
        while (k > 0 && sauv != NULL) {
            lane* new_lane = (lane*)malloc(sizeof(lane));
            new_lane->y = sauv->y;
            new_lane->speed = sauv->speed;
            new_lane->obst_size = sauv->obst_size;
            new_lane->type = sauv->type;
            new_lane->prev = NULL; //NULL for the moment, possible futur bug
            new_lane->obstacles = copy_obstacle(sauv->obstacles);
            new_lane->coins = NULL;
            new_lane->next = last_copied;
            last_copied->prev = new_lane;
            last_copied = new_lane;
            sauv = sauv->prev;
            k = k - 1;
        }

    return copy;
    }
    
}


bool collides_without_game(lane *current_lane, player p) {
    /* checks if the player collides with an obstacle */
    obstacle* current_obstacle = current_lane->obstacles;
    if (current_lane->type != 2) { // WATER == 2
        while (current_obstacle != NULL) {
            if (p.x >= current_obstacle->x && p.x < current_obstacle->x + (current_obstacle->size)) {
                return true;
            } else {
                current_obstacle = current_obstacle->next;
            }
       }
       return false;
    } else {
        while (current_obstacle != NULL) {
            if (p.x >= current_obstacle->x && p.x < current_obstacle->x + (current_obstacle->size)) {
                return false;
            } else {
                current_obstacle = current_obstacle->next;
            }
       }
       return true;
    }
    
}

void update_lanes(lane* l) {
    // update the lane
    lane* current_lane = l;
    while (current_lane != NULL) {
        switch (current_lane->type) {;
            case ROAD:
            update_vehicles(current_lane);
            break;
            case WATER:
            update_drowning_slots(current_lane);
            break;
            case TRACK:
            update_trains(current_lane);
            break;
            default:
            break;
        }

        current_lane = current_lane->next;
    }
}

couple minmax_rec(lane* l, int deep, couple previous, player p) {
    /* naive minmax
    parameters : l the current lane, deep the more deepest you want to test, previous the result of the previous call of minmax, p a player representing the ia
    return : a couple (score, move), where score is the max you can afford in terms of deplacement, ie not you real score but your progression and move, the move to do to obtain the score
    */
    /* explaining in French because I don't have my TOEIC
    cas deep <= 0 -> on a fini -> renvoyer le score (et le mouvement)
    le reste du temps
    effectuer un mouvement, tester s'il est juste (collision ou eau)
    s'il es juste, calculer le nouveau score (+1 si devant, -1 si derrière, 0 sinon)
    remonter ce score à une variable globale à la boucle qu'on met à jour si c'est mieux
    recommencer jusqu'à fin des mouvements possibles
    -> le meilleur mouvement est celui permis par la boucle lors du premier appel
    -> si on envoie un couple (score, mvt), c'est ce qu'on maintient à jour dans la boucle et qu'on renvoie
    */
    if (deep <= 0) {
        return previous;
    } else {
        // on a state, want to know possibilities
        couple res;
        res.move = NOT_POSSIBLE;
        res.score = -deep-1;

        // step 1 : update the map and do a move
        lane* current_lane = copy_lane(l, deep);
        lane* copy_current = current_lane;

        lane* beginning_lane = copy_current;
        while (copy_current->prev != NULL) {
            beginning_lane = beginning_lane->prev;
            copy_current= copy_current->prev;
        }

        update_lanes(beginning_lane);
        copy_current = current_lane;

        // update your position if you are on water
        if (current_lane->type == 2) {
            p.x = p.x + current_lane->speed;
        }

        // step 2 : try all the moves, which are represented by numbres, see top of the code
        player copy_p;
        copy_p.orientation = p.orientation;
        copy_p.skin = p.skin;
        for (int i=1; i<6; i=i+1) {
            // couple linked to a mouv
            couple c;
            c.move = i;
            c.score = previous.score;

            switch (i) {
                case GO_AHEAD:
                    if (l->next != NULL) {
                        copy_p.x = p.x;
                        copy_p.y = p.y + 1;
                        current_lane = copy_current->next;
                        c.score = c.score+1;
                    } else {
                        c.move = NOT_POSSIBLE;
                    }
                    break;
                case GO_DOWN:
                    if (l->prev != NULL) {
                        copy_p.x = p.x;
                        copy_p.y = p.y - 1;
                        current_lane =  copy_current->prev;
                        c.score = c.score-1;
                    } else {
                        c.move = NOT_POSSIBLE;
                    }
                break;
                case GO_LEFT:
                    if (p.x >= UNPLAYABLE_WIDTH) {
                        copy_p.x = p.x - 1;
                        copy_p.y = p.y;
                        current_lane = copy_current;
                    } else {
                        c.move = NOT_POSSIBLE;
                    }
                    break;
                case GO_RIGHT:
                    if (copy_p.x < LANE_WIDTH -1 - UNPLAYABLE_WIDTH ) {
                        copy_p.x = p.x + 1;
                        copy_p.y = p.y;
                        current_lane = copy_current;
                    } else {
                        c.move = NOT_POSSIBLE;
                    }
                    break;
                default:
                    current_lane = copy_current;
                    copy_p.x = p.x;
                    copy_p.y = p.y;
                    break;
            }
            

            // for each possible move, check the collision
            bool coll = collides_without_game(current_lane, copy_p);

            // step 3 : if move possible and no collision, continue with this move
            couple next;
            if (c.move != NOT_POSSIBLE && !coll) {
                next = minmax_rec(current_lane, deep-1, c, copy_p);
            } else {
                next.move = NOT_POSSIBLE;
                next.score = -1;
            }

            // step 4 : update res if c better than it
            if (next.move != NOT_POSSIBLE && next.score > res.score) {
                res.move = c.move;
                res.score = next.score;
            }

        }
        free_lanes(beginning_lane);
        return res;
    }
}

player minmax_simple(lane* l, int deep, player p) {
    /* parameters : the lane were the ai is, the deep, and an player representing the ai
        returns a player who has done the move found by minmax_rec
    */
    couple todo;
    todo.score = 0;
    todo.move = STAY;
    couple c = minmax_rec(l, deep, todo, p);
    switch (c.move) {
        case GO_AHEAD:
            p.y = p.y + 1;
            break;
        case GO_DOWN:
            p.y = p.y - 1;
            break;
        case GO_LEFT:
            p.x = p.x - 1;
            break;
        case GO_RIGHT:
            p.x = p.x + 1;
            break;
        default:
        break;
    }
    return p;
}


lane** n_update(int n, lane* l) {
    // makes n update of the lanes and store them in a array, the last update is in the first case
    // it updates the lanes since the beginning obtains by copy_lane() -> in tab[i] the 'current' lane, not the first
    lane** tab = (lane**)malloc(sizeof(lane*)*n);
    lane* current_lane = copy_lane(l, n);
    lane* beginning_lane = current_lane;

    for (int i=0; i<n; i=i+1) {
        while (beginning_lane->prev != NULL) {
            beginning_lane = beginning_lane->prev;
        }
        update_lanes(beginning_lane);
        tab[n-i-1] = current_lane;
        current_lane = copy_lane(current_lane, n);
        beginning_lane = current_lane;
    }

    while (beginning_lane->prev != NULL) {
        beginning_lane = beginning_lane->prev;
    }
    free_lanes(beginning_lane);
    
    return tab;
}

void free_update(lane** tab, int n) {
    // frees tab carefully
    for (int i=0; i<n; i=i+1) {
        lane* beginning_lane = tab[i];
        while (beginning_lane->prev != NULL) {
            beginning_lane = beginning_lane->prev;
        }
        free_lanes(beginning_lane);
    }
    free(tab);
}

couple minmax_rec_memo_state(lane* l, int deep, couple previous, player p, lane** tab) {
    /* minmaw but the deepth possible states of the map are known in tab, without the ai
    parameters : l the current lane, deep the more deepest you want to test, previous the result of the previous call of minmax, p a player representing the ia, tab the deepth next states of the map with the last in case 0
    return : a couple (score, move), where score is the max you can afford in terms of deplacement, ie not you real score but your progression and move, the move to do to obtain the score
    */
    /*cas deep <= 0 -> on a fini -> renvoyer le score (et le mouvement)
    le reste du temps
    effectuer un mouvement, tester s'il est juste (collision ou eau)
    s'il es juste, calculer le nouveau score (+1 si devant, -1 si derrière, 0 sinon) ?
    remonter ce score à une variable globale à la boucle qu'on met à jour si c'est mieux
    recommencer jusqu'à fin des mouvements possibles
    -> le meilleur mouvement est celui permis par la boucle lors du premier appel
    -> si on envoie un couple (score, mvt), c'est ce qu'on maintient à jour dans la boucle et qu'on renvoie
    */
    if (deep <= 0) {
        return previous;
    } else {
        // on a state, want to know possibiities
        couple res;
        res.move = NOT_POSSIBLE;
        res.score = -deep-1;

        // step 1 : update the map and do a move
        lane* current_lane = tab[deep-1];
        lane* copy_current = current_lane;

        // update your position if you are on water
        if (current_lane->type == 2) {
            p.x = p.x + current_lane->speed;
        }

        player copy_p;
        copy_p.orientation = p.orientation;
        copy_p.skin = p.skin;
        for (int i=1; i<6; i=i+1) {
            // couple linked to a mouv
            couple c;
            c.move = i;
            c.score = previous.score;
            bool ahead = false;
            bool back = false;

            switch (i) {
                case GO_AHEAD:
                    if (copy_current->next != NULL) {
                        copy_p.x = p.x;
                        copy_p.y = p.y + 1;
                        current_lane = copy_current->next;
                        ahead = true;
                        for (int j=0; j<deep; j=j+1) {
                            tab[j] = tab[j]->next;
                        }
                        c.score = c.score+1;
                    } else {
                        c.move = NOT_POSSIBLE;
                    }
                    break;
                case GO_DOWN:
                    if (copy_current->prev != NULL) {
                        copy_p.x = p.x;
                        copy_p.y = p.y - 1;
                        current_lane =  copy_current->prev;
                        back = true;
                        for (int j=0; j<deep; j=j+1) {
                            tab[j] = tab[j]->prev;
                        }
                        c.score = c.score-1;
                    } else {
                        c.move = NOT_POSSIBLE;
                    }
                break;
                case GO_LEFT:
                    if (p.x >= UNPLAYABLE_WIDTH) {
                        copy_p.x = p.x - 1;
                        copy_p.y = p.y;
                        current_lane = copy_current;
                    } else {
                        c.move = NOT_POSSIBLE;
                    }
                    break;
                case GO_RIGHT:
                    if (copy_p.x < LANE_WIDTH -1 - UNPLAYABLE_WIDTH ) {
                        copy_p.x = p.x + 1;
                        copy_p.y = p.y;
                        current_lane = copy_current;
                    } else {
                        c.move = NOT_POSSIBLE;
                    }
                    break;
                default:
                    current_lane = copy_current;
                    copy_p.x = p.x;
                    copy_p.y = p.y;
                    break;
            }
            

            // step 2 : for each possible move, check the collision
            bool coll = collides_without_game(current_lane, copy_p);

            // step 3 : if move possible and no collision, continu with this move
            couple next;
            if (c.move != NOT_POSSIBLE && !coll) {
                next = minmax_rec_memo_state(current_lane, deep-1, c, copy_p, tab);
            } else {
                next.move = NOT_POSSIBLE;
                next.score = -1;
            }

            // change tab if GO_AHEAD or GO_DOWN
            if (i==GO_AHEAD && ahead) {
                for (int j=0; j<deep; j=j+1) {
                    tab[j] = tab[j]->prev;
                }
                ahead = false;
            } else if (i==GO_DOWN && back) {
                for (int j=0; j<deep; j=j+1) {
                    tab[j] = tab[j]->next;
                }
                back = false;
            }

            // step 4 : update res if c better than him
            if (next.move != NOT_POSSIBLE && next.score > res.score) {
                res.move = c.move;
                res.score = next.score;
            }

        }
        return res;
    }
}

player minmax_memo_state(lane* l, int deep, player p) {
    /* parameters : the lane were the ai is, the deep, and an player representing the ai
        returns a player who has done the move found by minmax_rec
    */
    couple todo;
    todo.score = 0;
    todo.move = STAY;
    lane** tab = n_update(deep, l);
    couple c = minmax_rec_memo_state(l, deep, todo, p, tab);
    switch (c.move) {
        case GO_AHEAD:
            p.y = p.y + 1;
            break;
        case GO_DOWN:
            p.y = p.y - 1;
            break;
        case GO_LEFT:
            p.x = p.x - 1;
            break;
        case GO_RIGHT:
            p.x = p.x + 1;
            break;
        default:
        break;
    }
    free_update(tab, deep);
    return p;
}


player play_ai(int cai, lane* player_lane, player ai) {
    // make the move for the ai
    switch (cai) {
        case 1:
        return minmax_simple(player_lane, 5, ai);
        break;
        case 2:
        return minmax_memo_state(player_lane, 5, ai);
        break;
        case 4:
        return minmax_memo_state(player_lane, 5, ai);
        break;
        default:
        return ai;
        break;
    }
}
