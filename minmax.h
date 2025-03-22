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
obstacle* collides_without_game(lane *current_lane, player p);
couple minmax_rec(lane* l, int deep, int high_score, int previous_move, player p);
