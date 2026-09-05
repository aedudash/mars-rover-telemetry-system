// Planet.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Provides SOL length and season lookup (for Mars).

#pragma once

#include "Measurement.h"
#include "Types.h"

class Planet {
private:
    double solSeconds_; // seconds per sol
    int    yearSols_;   // sols per martian year

public:
    // PRE: none
    // POST: Initializes with given sol length and sols/year (defaults = Mars-like)
    Planet(double solSeconds = 88775.0, int yearSols = 669)
        : solSeconds_(solSeconds), yearSols_(yearSols) {}

    // PRE: none
    // POST: Returns SOL duration as a Measurement in seconds
    Measurement solLength() const;

    // PRE: solIndex may be any int (wraps internally)
    // POST: Returns the Season for that sol index
    Season seasonOf(int solIndex) const;

    // PRE: none
    // POST: Returns a Planet preconfigured for Mars
    static Planet Mars();
};

