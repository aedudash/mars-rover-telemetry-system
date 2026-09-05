// TemperatureStats.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Implementation for streaming temperature stats + season minima.
//          Tracks running mean/median, top/bottom extremes, and seasonal lows
//          from a stream of temperature samples (stored internally in °C).

#include "TemperatureStats.h"
#include <algorithm>

// Keep two heaps roughly balanced so we can pull a median in O(1).
// - lower: max-heap of values <= current median
// - upper: min-heap of values >= current median
static void rebalance(std::priority_queue<double>& lower,
                      std::priority_queue<double, std::vector<double>, std::greater<double>>& upper)
{
    if (lower.size() > upper.size() + 1) { upper.push(lower.top()); lower.pop(); }
    else if (upper.size() > lower.size()) { lower.push(upper.top()); upper.pop(); }
}

// Simple °C → K conversion helper for the reporting methods.
static double CtoK(double c) {
    return c + 273.15;
}

// Ingest one temperature sample, update aggregates and seasonal lows.
void TemperatureStats::ingest(Measurement tempK, int solIndex) {
    const double c = tempK.asBase().value();   // effectively °C in this setup
    sumK_ += c;
    ++count_;

    // Maintain streaming median via two heaps.
    if (lower_.empty() || c <= lower_.top()) lower_.push(c);
    else                                      upper_.push(c);
    rebalance(lower_, upper_);

    // Track top 3 hottest temperatures (stored in C).
    top3_.push_back(c);
    std::sort(top3_.begin(), top3_.end(), std::greater<double>());
    if (top3_.size() > 3) top3_.resize(3);

    // Track bottom 3 coldest temperatures (stored in C).
    bottom3_.push_back(c);
    std::sort(bottom3_.begin(), bottom3_.end());
    if (bottom3_.size() > 3) bottom3_.resize(3);

    // Update seasonal minima if we know how to map sol → season.
    if (planet_) {
        Season s = planet_->seasonOf(solIndex);
        if (s == Summer) lowSummerK_ = std::min(lowSummerK_, c);
        if (s == Winter) lowWinterK_ = std::min(lowWinterK_, c);
    }
}

// Placeholder hook in case we ever need end-of-stream work.
void TemperatureStats::finalize() {}

// Mean temperature in K (delegates to meanC).
double TemperatureStats::meanK() const {
    return CtoK(meanC());
}

// Median temperature in K (delegates to medianC).
double TemperatureStats::medianK() const {
    return CtoK(medianC());
}

// Mean temperature in °C (0 if no samples).
double TemperatureStats::meanC() const {
    return count_ ? (sumK_ / static_cast<double>(count_)) : 0.0;
}

// Median temperature in °C (0 if no samples).
double TemperatureStats::medianC() const {
    if (!count_) return 0.0;
    if (lower_.size() == upper_.size()) return (lower_.top() + upper_.top()) / 2.0;
    return lower_.top();
}

// Hottest three samples as Kelvin, sorted ascending.
std::vector<double> TemperatureStats::top3K() const {
    std::vector<double> v = top3_;          // values in C
    std::sort(v.begin(), v.end());          // ascending in C
    for (double &x : v) x = CtoK(x);        // convert to K for reporting
    return v;
}

// Coldest three samples as Kelvin, sorted ascending.
std::vector<double> TemperatureStats::bottom3K() const {
    std::vector<double> v = bottom3_;       // values in C, already ascending
    std::sort(v.begin(), v.end());          // ensure ascending
    for (double &x : v) x = CtoK(x);        // convert to K for reporting
    return v;
}

// Lowest Summer temperature in K.
// Caller treats +infinity as "none" if no samples were recorded.
double TemperatureStats::lowestSummerK() const {
    return CtoK(lowSummerK_);
}

// Lowest Winter temperature in K.
// Caller treats +infinity as "none" if no samples were recorded.
double TemperatureStats::lowestWinterK() const {
    return CtoK(lowWinterK_);
}
