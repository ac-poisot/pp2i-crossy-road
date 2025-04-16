#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "core.h"

#define LANE_WIDTH 24 // width of the displayed area
#define UNPLAYABLE_WIDTH 2

#define NOT_POSSIBLE 0
#define GO_AHEAD 1
#define GO_DOWN  2
#define GO_RIGHT 3
#define GO_LEFT  4
#define STAY     5

typedef struct couple_s {
    int score;
    int move;
} couple;

obstacle* copy_obstacle(obstacle* o);
lane* copy_lane(lane* l, int p);
void printf_lane(lane* l, player p) ;
bool collides_without_game(lane *current_lane, player p);
void update_lanes(lane* lane);
couple minmax_rec(lane* l, int deep, couple previous, player p);
lane** n_update(int n, lane* l);
void free_update(lane** tab, int n);
