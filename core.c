#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "core.h"

int tick;


void free_obstacles(obstacle* first_obstacle) {
    /* frees recursively obstacles */
    obstacle* current_obstacle = first_obstacle;
    obstacle* next_obstacle;
    while (current_obstacle != NULL) {
        next_obstacle = current_obstacle->next;
        free(current_obstacle);
        current_obstacle = next_obstacle;
    }
}

void free_lanes(lane* first_lane) {
    /* frees recursively lanes */
    lane* current_lane = first_lane;
    lane* next_lane;
    while (current_lane != NULL) {
        next_lane = current_lane->next;
        free(current_lane->coins);
        free_obstacles(current_lane->obstacles);
        free(current_lane);
        current_lane = next_lane;
    }
}


int biome(int* boules){// Sa prend les boules de chaque couleur, et renvoit la couleur pioché
    /* chooses randomly a number */
    int nbr_de_boule= boules[1] + boules[2] + boules[3] + boules[0];
    int boule_choisi =rand()%nbr_de_boule;
    //return boule_choisi;
    tick++;
    if (boule_choisi < boules[0]) {
        return 1;
    }
    else if (boule_choisi < boules[1]+boules[0]) {
        return 2;
    }
    else if (boule_choisi<boules[1]+boules[2]+boules[0]) {
        return 3;
    }
    else {
        return 4;
    }
}

int weight(int index) {
    /* weigth for probabilities depending on time */
    return (4-index) * tick;
}

int* probabilite_biomes(lane* l) {// prend les 4 lignes et renvoie les probabilité dans le panier
    /* creates the urn for the generation of biomes */
    int a = 1000;
    int b = 1000;
    int c = 1000;
    int d = 1000;
    for (int i=0;i<4;i++){
        if (l->type == 1) {
            a += weight(i);
        }
        else if (l->type == 2) {
            b += weight(i);
        }
        else if (l->type == 3) {
            c += weight(i);
        }
        else if (l->type == 4) {
            d += weight(i);
        }
        l = l->next;
    }
    int* boules = malloc(4*sizeof(int));
    boules[0] = a;
    boules[1] = b;
    boules[2] = c;
    boules[3] = d;
    return boules;
}// Cette version ne prend pas en compte l'avancée dans le temps


lane* empty_lane(lane* prev_lane, int type) {
    /* creates an empty lane */
    lane* new_lane = malloc(sizeof(lane));
    if (prev_lane != NULL) {
        new_lane->y = prev_lane->y + 1;
        prev_lane->next = new_lane;
    } else {
        new_lane->y = 0;
    }
    new_lane->prev = prev_lane;
    new_lane->speed = 0;
    new_lane->obstacles = NULL;
    new_lane->obst_size = 1+rand()%2;
    new_lane->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        new_lane->coins[i] = false;
    }
    new_lane->type = type;
    new_lane->next = NULL;
    
    return new_lane;
}


lane* initialLanes(void) {
    /* initializes the four first lanes */
    lane* l = empty_lane(NULL, GRASS);
    lane* firstLane = l;
    for (int i = 0; i < 4; i++) {
        l = empty_lane(l, GRASS);
    }
    return firstLane;
}


obstacle* generate_vehicles(void) {
    /* generates vehicles */
    float x = (float)(rand()%12);
    float current_x = x;
    obstacle* first_obst = (obstacle*)malloc(sizeof(obstacle));
    first_obst->x = current_x;
    first_obst->prev = NULL;
    first_obst->next = NULL;
    obstacle *last_generated = first_obst;
    while (current_x < LANE_WIDTH) {
        current_x += (float)(6*(rand()%2)+6);
        obstacle* obst = (obstacle*)malloc(sizeof(obstacle));
        obst->x = current_x;
        obst->next = NULL;
        obst->prev = last_generated;
        last_generated->next = obst;
        last_generated = obst;
    }
    return first_obst;
}


lane* generate_lane(lane* prev_lane, int type) {
    /* creates an empty lane */
    lane* new_lane = malloc(sizeof(lane));
    if (prev_lane != NULL) {
        new_lane->y = prev_lane->y + 1;
        prev_lane->next = new_lane;
    } else {
        new_lane->y = 0;
    }
    new_lane->prev = prev_lane;
    switch (type) {
        case GRASS:
        case WATER:
        case TRACK:
        case ROAD:
            new_lane->obstacles = generate_vehicles();
            new_lane->speed = 2;
            break;
    }
    new_lane->obst_size = 1+rand()%2;
    new_lane->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        new_lane->coins[i] = false;
    }
    new_lane->type = type;
    new_lane->next = NULL;
    
    return new_lane;
}


void update_vehicles(lane* l) {
    /* updates the position of the vehicles and add a new one when the next displayed vehicle doesn't exist */
    obstacle* c = l->obstacles;
    if (l->speed > 0 && c->x+l->speed > 6) {
        obstacle* new_obst = (obstacle*)malloc(sizeof(obstacle));
        float new_x = c->x - (float)(6*(rand()%2)+6);
        new_obst-> prev = NULL;
        new_obst-> next = c;
        new_obst->x = new_x;
        c->prev = new_obst;
        l->obstacles = new_obst;
        c = new_obst;
    }
    while (c != NULL) {
        if ((c->x + l->speed > LANE_WIDTH) && (l->speed > 0)){
            c-> prev ->next = NULL;
            free(c);
            break;
        } else if ((c->x + l->speed + l->obst_size < 1) && (l->speed < 0)){
            c->next->prev = NULL;
            obstacle* nc = c->next;
            free(c);
            c = nc;
            l->obstacles = c;
        } else if (c->next == NULL && l->speed < 0 && c->x+l->speed < LANE_WIDTH) {
            c->x = c->x + l->speed;
            obstacle* next_obst = (obstacle*)malloc(sizeof(obstacle));
            float current_x = c->x + (float)(6*(rand()%2)+6);
            next_obst->x = current_x;
            next_obst->next = NULL;
            next_obst->prev = c;
            c->next = next_obst;
            c = c->next;
            break;
        } else {
            c->x = c->x + l->speed;
            c = c ->next;
        }
    }
}

void display_obstacles(obstacle* o) {
    /* displays the position of the vehicles -- debbug function */
    while (o != NULL) {
        printf("%f\n", o->x);
        o = o->next;
    }
}

void displayLanes(lane* l) {
    /* displays the lanes */
    while (l != NULL) {
        obstacle* current = NULL;
        if(l->obstacles != NULL) {
            current = l->obstacles;
        }
        for (int i = 0; i < LANE_WIDTH; i++) {
            if(current != NULL && current->x == (float)i) {
                for(int j = 0; j < l->obst_size; j++) {
                    if(i >= LANE_WIDTH) {
                        break;
                    }
                    printf("🟥");
                    i++;
                }
                i--;
                current = current->next;
            } else {
                switch (l->type) {
                    case GRASS:
                        printf("🟩");
                        break;
                    case WATER:
                        printf("🟦");
                        break;
                    case TRACK:
                        printf("⬜️");
                        break;
                    case ROAD:
                        printf("⬛️");
                        break;
                }
            }

        }
        printf("\n");
        l = l->next;
    }
}

lane* fourLastLanes(lane* l) {
    /* gets the four previous lanes */
    lane* lastLane = l;
    for (int i = 0; i < 3; i++) {
        lastLane = lastLane->prev;
    }
    return lastLane;
}

void generateNNewLanes(lane* l, int n) {
    /* generates n lanes based on the four previous lanes */
    for (int i = 0; i < n; i++) {
        l = generate_lane(l, biome(probabilite_biomes(fourLastLanes(l))));
    }
}

int main(void) {
    srand(time(NULL));
    lane* firstLane = initialLanes();
    firstLane->speed = 2;
    //generateNNewLanes(firstLane->next->next->next, 100);
    
    //displayLanes(firstLane);

    obstacle* v1 = generate_vehicles();
    firstLane->obstacles = v1;
    firstLane->obst_size = -2;
    display_obstacles(firstLane->obstacles);
    printf("\n");
    for(int i=0; i<5; i=i+1) {
        update_vehicles(firstLane);
        display_obstacles(firstLane->obstacles);
        printf("\n");
    }
    //free_obstacles(v1); // ne pas free si v1 lie a firstLane

    free_lanes(firstLane);
    return 0;
}
