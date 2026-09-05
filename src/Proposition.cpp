// Proposition.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Implementation of logical proposition helpers and comparisons.
//          Handles case/whitespace cleanup, numeric tolerance, and basic
//          parsing from strings into enum-backed forms.

#include "Proposition.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <ostream>
#include <string>

namespace {
    // Return a lowercase copy of s with all whitespace removed.
    // Used to make parsing more forgiving (spaces in input don't matter).
    std::string lowerNoSpace(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for (char c : s) {
            if (std::isspace(static_cast<unsigned char>(c))) continue;
            out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
        return out;
    }

    // Simple lowercase helper that keeps spacing.
    std::string lower(const std::string& s) {
        std::string out = s;
        for (char& c : out) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return out;
    }

    // Compare two doubles with a small tolerance instead of exact equality.
    // This keeps tiny floating point noise from breaking structural matches.
    bool sameDouble(double a, double b, double eps = 1e-6) {
        return std::fabs(a - b) <= eps;
    }

    // String names for enums for human-readable output.
    const char* formName(PredicateForm f) {
        switch (f) {
            case PredicateForm::Detected: return "detected";
            case PredicateForm::All:      return "all";
            case PredicateForm::Some:     return "some";
            case PredicateForm::None:     return "none";
            case PredicateForm::Is:       return "is";
            case PredicateForm::Average:  return "average";
            default:                      return "unknown";
        }
    }

    const char* subjectName(Subject s) {
        switch (s) {
            case Subject::Water:        return "water";
            case Subject::Hydrogen:     return "hydrogen";
            case Subject::Temperature:  return "temperature";
            case Subject::Distance:     return "distance";
            case Subject::Life:         return "life";
            case Subject::MicrobialLife:return "microbial_life";
            case Subject::AncientLife:  return "ancient_life";
            default:                    return "unknown";
        }
    }

    const char* paramKindName(ParamKind pk) {
        switch (pk) {
            case ParamKind::None:       return "none";
            case ParamKind::Intensity:  return "intensity";
            case ParamKind::Distance:   return "distance";
            case ParamKind::Temperature:return "temperature";
            default:                    return "none";
        }
    }
}

// --- structural comparison ---

// Check if two propositions are structurally the same:
// same form, subject, parameter kind, (fuzzy) value, and same negation flag.
bool Proposition::equivalentTo(const Proposition& other) const {
    if (form_      != other.form_)      return false;
    if (subject_   != other.subject_)   return false;
    if (paramKind_ != other.paramKind_) return false;
    if (!sameDouble(value_, other.value_)) return false;
    if (negated_   != other.negated_)   return false;
    return true;
}

// Check if this proposition is the logical negation of another,
// but still talking about the same subject, form, and value.
bool Proposition::isNegationOf(const Proposition& other) const {
    if (form_      != other.form_)      return false;
    if (subject_   != other.subject_)   return false;
    if (paramKind_ != other.paramKind_) return false;
    if (!sameDouble(value_, other.value_)) return false;
    if (negated_   == other.negated_)   return false;
    return true;
}

// --- parsing helpers ---

// Map a loose, user-facing string to a PredicateForm.
// Case and whitespace are ignored so "  DeTeCtEd  " still works.
PredicateForm Proposition::predicateFromString(const std::string& name) {
    std::string n = lowerNoSpace(name);

    if (n == "detected")    return PredicateForm::Detected;
    if (n == "all")         return PredicateForm::All;
    if (n == "some")        return PredicateForm::Some;
    if (n == "no")          return PredicateForm::None;
    if (n == "is")          return PredicateForm::Is;
    if (n == "average")     return PredicateForm::Average;

    // Unknown predicate type; caller can treat this as a parse failure.
    // Could extend with "implies", "not_implies" etc. if needed later.
    return PredicateForm::Unknown;
}

// Map a loose, user-facing string to a Subject.
// Again, case and whitespace are ignored, and a few synonyms are allowed.
Subject Proposition::subjectFromString(const std::string& name) {
    std::string n = lowerNoSpace(name);

    if (n == "water")           return Subject::Water;
    if (n == "hydrogen")        return Subject::Hydrogen;
    if (n == "temperature")     return Subject::Temperature;
    if (n == "distance")        return Subject::Distance;
    if (n == "life")            return Subject::Life;
    if (n == "microbial" || n == "microbial_life")
        return Subject::MicrobialLife;
    if (n == "ancientlife" || n == "ancient_life" || n == "ancient")
        return Subject::AncientLife;

    // Anything that doesn't match a known subject drops into Unknown.
    return Subject::Unknown;
}

// --- stream inserter ---

// Produce a compact, human-readable description of the proposition.
// Example: P: NOT average(temperature, 248.000 K)
std::ostream& operator<<(std::ostream& os, const Proposition& p) {
    os << p.label() << ": ";
    if (p.negated()) {
        os << "NOT ";
    }

    os << formName(p.form()) << "(" << subjectName(p.subject());

    if (p.paramKind() != ParamKind::None) {
        os << ", " << p.value() << " " << paramKindName(p.paramKind());
    }

    os << ")";
    return os;
}
