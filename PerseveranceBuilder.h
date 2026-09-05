// PerseveranceBuilder.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Build Perseverance propositions (P..T) from Robot data.

#pragma once

#include <map>
#include <string>

#include "Proposition.h"

class Robot;

class PerseveranceBuilder {
public:
    // PRE: Robot has ingested data and run() has been called
    // POST: returns PerPropSet: label -> Proposition for P..T
    std::map<std::string, Proposition>
    buildFromRobot(const Robot& robot) const;
};
