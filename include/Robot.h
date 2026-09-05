// Robot.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Orchestrates Navigation, TemperatureStats, and SherlocClassifier.

#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "Planet.h"
#include "Navigation.h"
#include "TemperatureStats.h"
#include "SherlocClassifier.h"
#include "Sample.h"

class Robot {
private:
    Planet             planet_;    // composition (1)
    Navigation         nav_;       // composition (1)
    TemperatureStats   temps_;     // composition (1)
    SherlocClassifier  sherloc_;   // composition (1)
    std::vector<Sample> samples_;  // all spectral samples (wavelength + intensity + sol)

public:
    // PRE: none
    // POST: Initializes with Mars + TemperatureStats bound to Planet
    Robot() : planet_(Planet::Mars()), nav_(), temps_(&planet_), sherloc_(), samples_() {}

    // PRE: path is a readable file
    // POST: Loads commands/samples from file and updates subsystems
    void loadFile(const std::string& path);

    // PRE: none
    // POST: Finalizes any streaming computations
    void run();

    // PRE: stream is open and writable
    // POST: Writes a human-readable report of collected data
    void writeReport(std::ostream& os) const;

    // --- Read-only accessors for logic/analysis layers ---

    // PRE: none
    // POST: Returns const reference to all spectral samples
    const std::vector<Sample>& samples() const { return samples_; }

    // PRE: none
    // POST: Returns const reference to temperature statistics
    const TemperatureStats& temps() const { return temps_; }

    // PRE: none
    // POST: Returns const reference to navigation history
    const Navigation& nav() const { return nav_; }

    // PRE: none
    // POST: Returns const reference to SHERLOC classifier
    const SherlocClassifier& sherlocClassifier() const { return sherloc_; }
};
