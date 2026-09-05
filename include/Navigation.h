// Navigation.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Accumulates per-SOL net displacement (with time).

#pragma once
#include <vector>
#include "Types.h"

class Navigation {
private:
    double fx_m_ = 0.0;
    double fy_m_ = 0.0;
    double t_s_  = 0.0;
    std::vector<DailyDisplacement> days_;

public:
    void beginSOL();
    void addMove(Measurement distance, Dir d, Measurement time);
    void endSOL();
    const std::vector<DailyDisplacement>& days() const { return days_; }
};
