// Planet.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Implementation for Planet utilities.

#include "Planet.h"
#include "Unit.h"

// Return the SOL duration as a Measurement in seconds.
Measurement Planet::solLength() const {
    Unit::registerDefaultUnits();
    Unit s = Unit::fromString("s");
    return Measurement(solSeconds_, s);
}

// Divide the year into four equal seasons and map a sol index into one of them.
// solIndex is wrapped into [0, yearSols_) to handle negative or large indices.
Season Planet::seasonOf(int solIndex) const {
    if (yearSols_ <= 0) return Spring;
    int i = solIndex % yearSols_;
    if (i < 0) i += yearSols_;
    int q = yearSols_ / 4;
    if (i < q)       return Spring;
    if (i < 2 * q)   return Summer;
    if (i < 3 * q)   return Fall;
    return Winter;
}

// Convenience constructor for Mars defaults.
Planet Planet::Mars() { return Planet(88775.0, 669); }
