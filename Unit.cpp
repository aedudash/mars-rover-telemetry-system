// Unit.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 2
// Purpose: Unit registry + parsing with SI prefixes, word forms, affine temps, and "sol".
//          Handles unit normalization, lookup by alias, and simple unit composition.

#include "Unit.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

using E = Unit::Exps;

// Registry record (defined here to avoid incomplete-type issues in the header).
// Each entry holds a canonical Unit plus a list of normalized spellings we accept.
struct UnitEntry {
    Unit u;
    std::vector<std::string> aliases; // accepted spellings (lowercased/normalized)
};

// ---- Base-dimension helpers -------------------------------------------------
// Convenience constructors for the seven SI base dimensions.
// Each one returns an exponent vector with a single 1 in the right slot.
static constexpr E M()   { return E{ 1,0,0,0,0,0,0 }; }
static constexpr E KG()  { return E{ 0,1,0,0,0,0,0 }; }
static constexpr E S()   { return E{ 0,0,1,0,0,0,0 }; }
static constexpr E A()   { return E{ 0,0,0,1,0,0,0 }; }
static constexpr E K()   { return E{ 0,0,0,0,1,0,0 }; }
static constexpr E MOL() { return E{ 0,0,0,0,0,1,0 }; }
static constexpr E CD()  { return E{ 0,0,0,0,0,0,1 }; }

// ---- Printing ---------------------------------------------------------------
// Return the canonical name for this Unit (as registered).
std::string Unit::toString() const { return name_; }

// Render an exponent vector into something like "m^1 s^-2" or "1" for dimensionless.
std::string Unit::expsToString(const Exps& e) {
    static const char* names[7] = {"m","kg","s","A","K","mol","cd"};
    std::ostringstream os;
    bool first = true;
    for (int i = 0; i < 7; ++i) {
        if (e[i] != 0) {
            if (!first) os << " ";
            os << names[i] << "^" << e[i];
            first = false;
        }
    }
    if (first) os << "1"; // dimensionless
    return os.str();
}

// Check whether two exponent vectors describe the same dimensions.
bool Unit::sameDims_(const Exps& a, const Exps& b) {
    for (int i = 0; i < 7; ++i) if (a[i] != b[i]) return false;
    return true;
}

// ---- Registry access --------------------------------------------------------
// Global registry of known units, created on first use.
std::vector<UnitEntry>& Unit::registry_() {
    static std::vector<UnitEntry> reg;
    return reg;
}

// Helper to stash a normalized alias spelling on a registry entry.
static void addAlias(UnitEntry& e, const std::string& a) { e.aliases.push_back(a); }

// Private static member wrapper so we can build entries from here.
// Each call creates one Unit plus a bundle of normalized alias strings.
void Unit::addEntry_(const Unit& u, std::initializer_list<const char*> aliases) {
    UnitEntry e{u,{}};
    for (auto* a : aliases) addAlias(e, Unit::norm_(a));
    Unit::registry_().push_back(e);
}

// ---- Registration -----------------------------------------------------------
// Populate the registry with the units this project expects to handle.
// Safe to call multiple times; the first call wins.
void Unit::registerDefaultUnits() {
    auto& R = registry_();
    if (!R.empty()) return; // already registered

    // Base SI
    addEntry_(Unit("m",   M(),   1.0), {"m","meter","meters","metre","metres"});
    addEntry_(Unit("kg",  KG(),  1.0), {"kg","kilogram","kilograms"});
    addEntry_(Unit("s",   S(),   1.0), {"s","sec","secs","second","seconds"});
    addEntry_(Unit("A",   A(),   1.0), {"A","ampere","amperes","amp","amps"});
    addEntry_(Unit("K",   K(),   1.0), {"K","kelvin","kelvins"});
    addEntry_(Unit("mol", MOL(), 1.0), {"mol","mole","moles"});
    addEntry_(Unit("cd",  CD(),  1.0), {"cd","candela","candelas"});

    // Length (word + symbol)
    addEntry_(Unit("mm", M(), 1e-3), {"mm","millimeter","millimeters"});
    addEntry_(Unit("cm", M(), 1e-2), {"cm","centimeter","centimeters"});
    addEntry_(Unit("um", M(), 1e-6), {"um","micrometer","micrometers","micron","microns"});
    addEntry_(Unit("nm", M(), 1e-9), {"nm","nanometer","nanometers"});
    addEntry_(Unit("km", M(), 1e3),  {"km","kilometer","kilometers"});

    // Time conveniences
    addEntry_(Unit("min", S(), 60.0),   {"min","minute","minutes"});
    addEntry_(Unit("h",   S(), 3600.0), {"h","hr","hour","hours"});
    addEntry_(Unit("sol", S(), 88775.0),{"sol","sols"});

    // Affine temperatures (base = Kelvin)
    // °C: K = C + 273.15
    addEntry_(Unit("C",   K(), 1.0, 273.15), {"c","celsius","degree celsius","degrees celsius"});
    // °F: K = (F - 32)*5/9 + 273.15 = F*(5/9) + (273.15 - 32*5/9)
    const double fFactor = 5.0/9.0;
    const double fBias   = 273.15 - 32.0 * fFactor;
    addEntry_(Unit("F",   K(), fFactor, fBias), {"f","fahrenheit","degree fahrenheit","degrees fahrenheit"});
}

// ---- Normalization helpers --------------------------------------------------
// Normalize a raw unit string: trim, lowercase, and collapse internal spaces.
std::string Unit::norm_(const std::string& s) {
    // trim + lowercase + collapse inner spaces
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    std::string t = (a == std::string::npos) ? std::string() : s.substr(a, b - a + 1);
    for (auto& c : t) c = (char)std::tolower((unsigned char)c);
    std::string out; out.reserve(t.size());
    bool prevSpace = false;
    for (char c : t) {
        bool sp = (c==' '||c=='\t');
        if (sp) { if (!prevSpace) out.push_back(' '); }
        else out.push_back(c);
        prevSpace = sp;
    }
    return out;
}

// Strip a simple plural 's' off the end (unless it's already "ss").
// This lets "meters" and "meter" normalize to the same key.
std::string Unit::singular_(const std::string& s) {
    if (s.size() >= 2 && s.back()=='s' && s[s.size()-2] != 's') {
        return s.substr(0, s.size()-1);
    }
    return s;
}

// ---- Parsing ---------------------------------------------------------------
// Convert a user-facing token into a Unit by probing the registry.
// This respects synonyms, case, spacing, µ vs u, and simple plurals.
Unit Unit::fromString(const std::string& token) {
    registerDefaultUnits();
    std::string q = singular_(norm_(token));

    // Normalize micro sign µ → u for consistency with "um" aliases.
    for (auto& c : q) if ((unsigned char)c == 0xB5) c = 'u';

    // Try direct alias match first.
    for (const auto& e : registry_()) {
        for (const auto& a : e.aliases) if (a == q) return e.u;
    }

    // Next, try matching with spaces removed (e.g., "degree celsius").
    std::string nospace = q;
    nospace.erase(std::remove(nospace.begin(), nospace.end(), ' '), nospace.end());
    for (const auto& e : registry_()) {
        for (const auto& a : e.aliases) {
            std::string an = a;
            an.erase(std::remove(an.begin(), an.end(), ' '), an.end());
            if (an == nospace) return e.u;
        }
    }

    // If we get here, the unit is not known to the registry.
    throw std::invalid_argument("unknown unit: " + token);
}

// ---- Composition ------------------------------------------------------------
// Multiply two units: add exponents and multiply scale factors.
// Result is purely multiplicative, so any affine bias is dropped.
Unit Unit::multiply(const Unit& a, const Unit& b) {
    Exps e{};
    for (int i=0;i<7;++i) e[i] = a.exps_[i] + b.exps_[i];
    // composed units cannot keep affine bias
    return Unit("", e, a.factorToBase_ * b.factorToBase_, 0.0);
}

// Divide two units: subtract exponents and divide scale factors.
// Again, the result is treated as a non-affine unit.
Unit Unit::divide(const Unit& a, const Unit& b) {
    Exps e{};
    for (int i=0;i<7;++i) e[i] = a.exps_[i] - b.exps_[i];
    return Unit("", e, a.factorToBase_ / b.factorToBase_, 0.0);
}
