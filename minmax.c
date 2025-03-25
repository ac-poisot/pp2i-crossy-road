#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "minmax.h"

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
        obstacle* o_cp = o;
        while (o_cp != NULL) {
            obstacle* new = (obstacle*)malloc(sizeof(obstacle));
            new->x = o->x;
            new->size = o->size;
            new->next = NULL;
            last_copy->next = new;
            new->prev = last_copy;
            last_copy = new;
            o_cp = o_cp->next;
        }
        return copy;
    }
}

lane* copy_lane(lane* l, int p) {
    // deep copy of the p lanes in front of and behind the player but without coins
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
        lane* sauv = l;
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
            k = k - 1;
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

obstacle* collides_without_game(lane *current_lane, player p) {
    /* checks if the player collides with an obstacle */
    obstacle* current_obstacle = current_lane->obstacles;
    printf("bele\n");
    while (current_obstacle != NULL) {
        printf("dehe\n");
        if (p.x+1 > current_obstacle->x && p.x < current_obstacle->x + (current_obstacle->size)) {
            return current_obstacle;
        } else {
            current_obstacle = current_obstacle->next;
        }
    }
    return NULL;
}


couple minmax_rec(lane* l, int deep, int high_score, int previous_move, player p) {
    couple c;
    // returns a couple composed of the highscore and the move
    if (deep <= 0 || l == NULL ) {
        c.score = high_score;
        c.move = previous_move;
        return c;
    } else {
        //update the map then do the five case
        lane* current_lane = copy_lane(l,deep);
        lane* very_first_of_current = current_lane;
        while (very_first_of_current->prev != NULL) {
            very_first_of_current = very_first_of_current->prev;
        }
        lane* player_lane = l;
        while (current_lane != NULL && current_lane->next != NULL) {
            // Update lanes
            switch (current_lane->type) {
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
            if (current_lane->y == p.y) {
                player_lane = current_lane;
            }
            current_lane = current_lane->next;

            // Check for collisions with obstacles
            obstacle* collided_obstacle = collides_without_game(player_lane, p);
            bool drown_flag = false;
            obstacle* on_log = NULL;
            if(collided_obstacle != NULL) {
                switch (player_lane->type) {
                    case GRASS:
                    break;
                    case TRACK:
                    c.score = high_score;
                    c.move = NOT_POSSIBLE;
                    free_lanes(very_first_of_current);
                    return c;
                    break;
                    case ROAD:
                    c.score = high_score;
                    c.move = NOT_POSSIBLE;
                    free_lanes(very_first_of_current);
                    return c;
                    break;
                    case WATER:
                    drown_flag = false;
                    if (player_lane->speed != 0) {
                        if (on_log != collided_obstacle) {
                            on_log = collided_obstacle;
                            p.x = collided_obstacle->x + abs((int) round(p.x - collided_obstacle->x));
                        }
                    }
                    break;
                    default:
                    break;
                }
            }



            if (drown_flag || p.x < UNPLAYABLE_WIDTH || p.x > LANE_WIDTH - UNPLAYABLE_WIDTH - 1) { // manque le cas ou on est hors champ en bas
                c.score = high_score;
                c.move = NOT_POSSIBLE;
                free_lanes(very_first_of_current);
                return c;
            }
        }

        // case
        couple c_up = minmax_rec(l->next, deep-1, high_score+1, GO_AHEAD, p);
        couple c_down = minmax_rec(l->prev, deep-1, high_score, GO_DOWN, p);
        couple c_left = minmax_rec(l, deep-1, high_score, GO_LEFT, p);
        couple c_right = minmax_rec(l, deep-1, high_score, GO_RIGHT, p);
        couple c_stay = minmax_rec(l, deep-1, high_score, STAY, p);

        int m = fmax(c_stay.score,fmax(fmax(c_up.score, c_down.score), fmax(c_left.score, c_right.score)));
        int move = NOT_POSSIBLE;

        if (c_up.move != NOT_POSSIBLE && c_up.score==m) {
            m = c_up.score;
            move = GO_AHEAD;
        }
        if (c_down.move != NOT_POSSIBLE && c_down.score==m) {
            m = c_down.score;
            move = GO_DOWN;
        }
        if (c_left.move != NOT_POSSIBLE && c_left.score==m) {
            m = c_left.score;
            move = GO_LEFT;
        }
        if (c_right.move != NOT_POSSIBLE && c_right.score==m) {
            m = c_right.score;
            move = GO_RIGHT;
        }
        if (c_stay.move != NOT_POSSIBLE && c_stay.score==m) {
            m = c_stay.score;
            move = STAY;
        }

        couple res;
        res.score = m;
        res.move = move;
        free_lanes(very_first_of_current);
        return res;

    }
}
