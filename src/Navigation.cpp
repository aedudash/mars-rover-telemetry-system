// Navigation.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: 2D displacement accumulation with time.
//          Treats each SOL as a net vector in meters with a heading angle.

#include "Navigation.h"
#include "Unit.h"
#include <cmath>

// Map discrete directions into compass-like headings in degrees.
// 0° = Right (east), 90° = Forward (north), 180° = Left (west), 270° = Backward (south).
static double heading_deg_for(Dir d) {
    switch (d) {
        case Right:    return 0.0;
        case Forward:  return 90.0;
        case Left:     return 180.0;
        case Backward: return 270.0;
    }
    return 0.0;
}

// Start a new SOL accumulation by clearing the running x/y components and time.
void Navigation::beginSOL() { fx_m_ = fy_m_ = t_s_ = 0.0; }

// Add one move to the current SOL vector, in whatever units distance/time use.
// Distance is converted to base meters, time to base seconds, then distance is
// projected onto x/y via heading.
void Navigation::addMove(Measurement distance, Dir d, Measurement time) {
    const double r   = distance.asBase().value();
    const double th  = heading_deg_for(d) * M_PI / 180.0;
    fx_m_ += r * std::cos(th);
    fy_m_ += r * std::sin(th);
    t_s_  += time.asBase().value();
}

// Finish the current SOL: collapse x/y into magnitude + heading, attach total time,
// and store it. Resets the accumulator so the next SOL can start fresh.
void Navigation::endSOL() {
    Unit::registerDefaultUnits();
    Unit m = Unit::fromString("m");
    Unit s = Unit::fromString("s");
    const double r  = std::hypot(fx_m_, fy_m_);
    const double th = std::atan2(fy_m_, fx_m_) * 180.0 / M_PI;
    Measurement mag(r, m);
    Measurement totalTime(t_s_, s);
    days_.push_back(DailyDisplacement{ mag, th, totalTime });
    fx_m_ = fy_m_ = t_s_ = 0.0;
}
