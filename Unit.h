// Unit.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Unit registry + helpers with SI prefixes, word/symbol forms, and affine temps.

#pragma once

#include <array>
#include <string>
#include <vector>

// Forward declaration for the internal registry record (defined in Unit.cpp)
struct UnitEntry;

class Unit {
public:
    using Exps = std::array<int, 7>; // m, kg, s, A, K, mol, cd

private:
    std::string name_;
    Exps        exps_{};
    double      factorToBase_ = 1.0;
    double      biasToBase_   = 0.0;

public:
    Unit() = default;
    Unit(const std::string& name, const Exps& e, double factorToBase,
         double biasToBase = 0.0)
        : name_(name), exps_(e), factorToBase_(factorToBase), biasToBase_(biasToBase) {}

    const std::string& name() const { return name_; }
    const Exps&        exponents() const { return exps_; }
    double             factorToBase() const { return factorToBase_; }
    double             biasToBase() const { return biasToBase_; }

    double toBase(double value) const { return (value + biasToBase_) * factorToBase_; }
    double fromBase(double baseVal) const { return baseVal / factorToBase_ - biasToBase_; }

    std::string toString() const;

    static void registerDefaultUnits();
    static Unit fromString(const std::string& token);
    static Unit fromSymbol(const std::string& sym) { return fromString(sym); }

    static std::string expsToString(const Exps& e);
    static Unit multiply(const Unit& a, const Unit& b);
    static Unit divide(const Unit& a, const Unit& b);

private:
    // internal registry + helpers
    static std::vector<UnitEntry>& registry_();
    static std::string norm_(const std::string& s);
    static std::string singular_(const std::string& s);
    static bool sameDims_(const Exps& a, const Exps& b);

    static void addEntry_(const Unit& u, std::initializer_list<const char*> aliases);
};

