// Measurement.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Scalar value + unit with base conversion and safe ops.

#pragma once

#include <string>
#include "Unit.h"

class Measurement {
private:
    double value_; // numeric in this unit
    Unit   unit_;

public:
    Measurement() : value_(0.0), unit_(Unit("1", Unit::Exps{}, 1.0)) {}
    Measurement(double value, const Unit& unit) : value_(value), unit_(unit) {}

    double value() const { return value_; }
    const Unit& unit() const { return unit_; }

    // PRE: none
    // POST: Returns a string "value unit"
    std::string toString() const;

    // PRE: none
    // POST: Returns same quantity expressed in base units
    Measurement asBase() const;

    // PRE: none
    // POST: Returns numeric value expressed in base units
    double baseValue() const;

    // PRE: none
    // POST: Returns true iff dimensions match
    bool sameDimensions(const Measurement& rhs) const;

    // PRE: target has same dimensions; throws otherwise
    // POST: Returns same quantity in target unit (handles affine temps)
    Measurement to(const Unit& target) const;

    // Arithmetic (dimension-checked add/sub, dim-composed mul/div)
    Measurement add(const Measurement& rhs) const;     // same dims
    Measurement subtract(const Measurement& rhs) const;// same dims
    Measurement multiply(const Measurement& rhs) const;// dims add
    Measurement divide(const Measurement& rhs) const;  // dims subtract

    // Dimension-checked comparisons (via base units)
    bool operator==(const Measurement& rhs) const;
    bool operator!=(const Measurement& rhs) const;
    bool operator<(const Measurement& rhs) const;
    bool operator<=(const Measurement& rhs) const;
    bool operator>(const Measurement& rhs) const;
    bool operator>=(const Measurement& rhs) const;
};
