// Types.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Shared enums and POD struct.

#pragma once
#include "Measurement.h"

enum Dir { Right, Left, Forward, Backward };
enum Season { Spring, Summer, Fall, Winter };

struct DailyDisplacement {
    Measurement magnitude;
    double      theta_deg;
    Measurement total_time;
};
