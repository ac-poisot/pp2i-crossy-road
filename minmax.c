#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "minmax.h"

obstacle* copy_obstacle(obstacle* o) {
    // deep copy of the obstacles
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

lane* copy_lane(lane* l, int p) {
    // deep copy of the p lanes in front of and behind the player
    lane* copy = (lane*)malloc(sizeof(lane));
    copy->y = l->y;
    copy->speed = l->speed;
    copy->obst_size = l->obst_size;
    copy->type = l->type;
    copy->prev = NULL;
    copy->next = NULL;
    copy->obstacles = copy_obstacle(l->obstacles);
    copy->coins = l->coins; // coins don't change for the moment, possible futur bug
    lane* sauv = l;
    lane* last_copied = copy;
    int k = p;
    // copies the next p lanes or stops before
    while (k > 0 && sauv != NULL) {
        lane* new_lane = (lane*)malloc(sizeof(lane));
        new_lane->y = sauv->y;
        new_lane->speed = sauv->speed;
        new_lane->obst_size = sauv->obst_size;
        new_lane->type = sauv->type;
        new_lane->next = NULL;
        new_lane->obstacles = copy_obstacle(sauv->obstacles);
        new_lane->coins = sauv->coins;
        new_lane->prev = last_copied;
        last_copied->next = new_lane;
        last_copied = new_lane;
        sauv = sauv->next;
        k = k - 1;
    }
    sauv = l;
    last_copied = copy;
    k = p;
    // copies the previous p lanes or stops befores
    while (k > 0 && sauv != NULL) {
        lane* new_lane = (lane*)malloc(sizeof(lane));
        new_lane->y = sauv->y;
        new_lane->speed = sauv->speed;
        new_lane->obst_size = sauv->obst_size;
        new_lane->type = sauv->type;
        new_lane->prev = NULL;
        new_lane->obstacles = copy_obstacle(sauv->obstacles);
        new_lane->coins = sauv->coins;
        new_lane->next = last_copied;
        last_copied->prev = new_lane;
        last_copied = new_lane;
        sauv = sauv->prev;
        k = k - 1;
    }
    return copy;
}
