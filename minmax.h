#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "core.h"

#define GO_AHEAD 1
#define GO_BACK  2
#define GO_RIGHT 3
#define GO_LEFT  4
#define STAY     5

obstacle* copy_obstacle(obstacle* o);
lane* copy_lane(lane* l, int p);
