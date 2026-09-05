// Proposition.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Logical proposition tagged by label (P..T) with subject,
//          predicate form, numeric parameter, and optional negation.

#pragma once

#include <string>
#include <iosfwd>

enum class PredicateForm {
    Detected,   // detected(subject, threshold)
    All,        // all(subject, threshold)
    Some,       // some(subject, threshold)
    None,       // no(subject, threshold)
    Is,         // is(subject, predicate) - not used numerically here
    Average,    // average(subject, temperature)
    Unknown
};

enum class Subject {
    Water,
    Hydrogen,
    Temperature,
    Distance,
    Life,
    MicrobialLife,
    AncientLife,
    Unknown
};

enum class ParamKind {
    None,
    Intensity,   // scalar intensity threshold
    Distance,    // meters
    Temperature  // Kelvin
};

class Proposition {
private:
    std::string  label_;     // e.g., "P", "Q", ...
    PredicateForm form_;
    Subject       subject_;
    ParamKind     paramKind_;
    double        value_;    // canonical numeric (K for temp, m for dist, raw for intensity)
    bool          negated_;  // true if outer "not(...)" or "no" style semantics

public:
    Proposition()
        : label_(),
          form_(PredicateForm::Unknown),
          subject_(Subject::Unknown),
          paramKind_(ParamKind::None),
          value_(0.0),
          negated_(false)
    {}

    Proposition(const std::string& label,
                PredicateForm form,
                Subject subject,
                ParamKind pk,
                double value,
                bool negated)
        : label_(label),
          form_(form),
          subject_(subject),
          paramKind_(pk),
          value_(value),
          negated_(negated)
    {}

    // --- accessors ---
    const std::string& label()     const { return label_; }
    PredicateForm      form()      const { return form_; }
    Subject            subject()   const { return subject_; }
    ParamKind          paramKind() const { return paramKind_; }
    double             value()     const { return value_; }
    bool               negated()   const { return negated_; }

    // --- mutators (used by PropositionSystem) ---
    void setNegated(bool n)            { negated_ = n; }
    void setParamKind(ParamKind pk)    { paramKind_ = pk; }
    void setValue(double v)            { value_ = v; }

    // --- structural comparison helpers ---

    // PRE: none
    // POST: returns true if this and other represent the same logical content
    //       (same form, subject, param kind, numeric value within tolerance,
    //        and same negation flag; label is ignored)
    bool equivalentTo(const Proposition& other) const;

    // PRE: none
    // POST: returns true if this and other are logical negations:
    //       same underlying structure but opposite negation flags
    bool isNegationOf(const Proposition& other) const;

    // --- string → enum helpers used by parsing layer ---

    // PRE: name is lowercased keyword like "detected", "all", "average", etc.
    // POST: returns corresponding PredicateForm, or Unknown if not recognized
    static PredicateForm predicateFromString(const std::string& name);

    // PRE: name is lowercased subject like "water", "hydrogen", "temperature", etc.
    // POST: returns corresponding Subject, or Unknown if not recognized
    static Subject subjectFromString(const std::string& name);
};

// PRE: none
// POST: streams a human-readable description of the proposition
std::ostream& operator<<(std::ostream& os, const Proposition& p);
