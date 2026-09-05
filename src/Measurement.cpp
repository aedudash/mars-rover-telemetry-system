// Measurement.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Implementation of value + unit with conversion and checked ops.

#include "Measurement.h"
#include <sstream>
#include <stdexcept>

// Pretty-print with fixed 3-decimal precision and the unit name.
std::string Measurement::toString() const {
    std::ostringstream os;
    os.setf(std::ios::fixed); os.precision(3);
    os << value_ << " " << unit_.toString();
    return os.str();
}

// Return an equivalent measurement expressed in the base unit for this dimension
// (e.g. meters, Kelvin), with a "bare" unit that just carries the exponents.
Measurement Measurement::asBase() const {
    return Measurement(unit_.toBase(value_), Unit("", unit_.exponents(), 1.0));
}

// Convenience: just the numeric value in base units.
double Measurement::baseValue() const {
    return unit_.toBase(value_);
}

// Dimension check: same underlying exponents?
bool Measurement::sameDimensions(const Measurement& rhs) const {
    return unit_.exponents() == rhs.unit_.exponents();
}

// Convert to a target unit with the same dimensions, via base units.
Measurement Measurement::to(const Unit& target) const {
    // Check dimensions first.
    if (unit_.exponents() != target.exponents()) {
        throw std::invalid_argument("incompatible unit conversion");
    }
    // Convert via base representation.
    const double base = unit_.toBase(value_);
    const double tgt  = target.fromBase(base);
    return Measurement(tgt, target);
}

// Add two compatible measurements. Work in base units, return in LHS unit.
Measurement Measurement::add(const Measurement& rhs) const {
    if (!sameDimensions(rhs))
        throw std::invalid_argument("add: incompatible dimensions");
    const double a = unit_.toBase(value_);
    const double b = rhs.unit_.toBase(rhs.value_);
    const double sumBase = a + b;
    const double out = unit_.fromBase(sumBase);
    return Measurement(out, unit_);
}

// Subtract two compatible measurements. Work in base units, return in LHS unit.
Measurement Measurement::subtract(const Measurement& rhs) const {
    if (!sameDimensions(rhs))
        throw std::invalid_argument("subtract: incompatible dimensions");
    const double a = unit_.toBase(value_);
    const double b = rhs.unit_.toBase(rhs.value_);
    const double diffBase = a - b;
    const double out = unit_.fromBase(diffBase);
    return Measurement(out, unit_);
}

// Multiply two measurements: multiply values and compose units.
Measurement Measurement::multiply(const Measurement& rhs) const {
    Unit outU = Unit::multiply(unit_, rhs.unit_);
    return Measurement(value_ * rhs.value_, outU);
}

// Divide two measurements: divide values and subtract unit exponents.
Measurement Measurement::divide(const Measurement& rhs) const {
    Unit outU = Unit::divide(unit_, rhs.unit_);
    return Measurement(value_ / rhs.value_, outU);
}

// All comparisons require compatible dimensions and are done in base units.
bool Measurement::operator==(const Measurement& rhs) const {
    if (!sameDimensions(rhs))
        throw std::invalid_argument("compare: incompatible dimensions");
    return baseValue() == rhs.baseValue();
}

bool Measurement::operator!=(const Measurement& rhs) const {
    return !(*this == rhs);
}

bool Measurement::operator<(const Measurement& rhs) const {
    if (!sameDimensions(rhs))
        throw std::invalid_argument("compare: incompatible dimensions");
    return baseValue() < rhs.baseValue();
}

bool Measurement::operator<=(const Measurement& rhs) const {
    if (!sameDimensions(rhs))
        throw std::invalid_argument("compare: incompatible dimensions");
    return baseValue() <= rhs.baseValue();
}

bool Measurement::operator>(const Measurement& rhs) const {
    if (!sameDimensions(rhs))
        throw std::invalid_argument("compare: incompatible dimensions");
    return baseValue() > rhs.baseValue();
}

bool Measurement::operator>=(const Measurement& rhs) const {
    if (!sameDimensions(rhs))
        throw std::invalid_argument("compare: incompatible dimensions");
    return baseValue() >= rhs.baseValue();
}
