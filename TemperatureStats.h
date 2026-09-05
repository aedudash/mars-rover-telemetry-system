// TemperatureStats.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Streaming stats for temperatures by SOL, with season minima.

#pragma once

#include <queue>
#include <vector>
#include <limits>
#include "Measurement.h"
#include "Types.h"
#include "Planet.h"

class TemperatureStats {
private:
    // Internally we store temperatures in Celsius and convert to K for reporting.
    double      sumK_   = 0.0;    // sum of samples in C
    std::size_t count_  = 0;

    // Running median (lower=max-heap, upper=min-heap), values in C
    std::priority_queue<double> lower_;
    std::priority_queue<double, std::vector<double>, std::greater<double>> upper_;

    // Keep top/bottom 3 (values in C)
    std::vector<double> top3_;
    std::vector<double> bottom3_;

    // Stored in C; +infinity means "no samples seen" (report prints this as "none")
    double lowSummerK_ = std::numeric_limits<double>::infinity(); // in C
    double lowWinterK_ = std::numeric_limits<double>::infinity(); // in C

    const Planet* planet_ = nullptr; // used to query Season

public:
    // PRE: planet may be null (season minima disabled in that case)
    // POST: Stores planet pointer for season queries
    explicit TemperatureStats(const Planet* planet = nullptr) : planet_(planet) {}

    // PRE: tempK is a temperature Measurement; solIndex any int
    // POST: Updates all streaming statistics
    void ingest(Measurement tempK, int solIndex);

    // PRE: none
    // POST: Finalization hook (no-op but kept for symmetry with UML)
    void finalize();

    // PRE: none
    // POST: Returns arithmetic mean in K
    double meanK() const;

    // PRE: none
    // POST: Returns median in K
    double medianK() const;

    // PRE: none
    // POST: Returns arithmetic mean in C
    double meanC() const;

    // PRE: none
    // POST: Returns median in C
    double medianC() const;

    // PRE: none
    // POST: Returns (up to) largest 3 samples in K (ascending)
    std::vector<double> top3K() const;

    // PRE: none
    // POST: Returns (up to) smallest 3 samples in K (ascending)
    std::vector<double> bottom3K() const;

    // PRE: none
    // POST: Returns lowest K recorded during Summer; if no samples, a +infinity
    //       sentinel is returned and the report layer prints "none".
    double lowestSummerK() const;

    // PRE: none
    // POST: Returns lowest K recorded during Winter; if no samples, a +infinity
    //       sentinel is returned and the report layer prints "none".
    double lowestWinterK() const;
};
