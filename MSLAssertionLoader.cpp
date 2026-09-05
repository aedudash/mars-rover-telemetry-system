// MSLAssertionLoader.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Implementation of MSL assertion parsing.
//          Reads lab-style assertions and turns them into Proposition objects
//          with predicate, subject, parameter kind, and value.

#include "MSLAssertionLoader.h"

#include <cctype>
#include <cmath>
#include <sstream>

// Trim leading and trailing whitespace from a string.
std::string MSLAssertionLoader::trim(const std::string& s) {
    std::size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return std::string();
    std::size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Lowercase input and drop all whitespace.
// Used to make matching "not(...)" and units more forgiving.
std::string MSLAssertionLoader::lowerNoSpace(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) continue;
        out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return out;
}

// Parse all MSL assertions from a stream into a label→Proposition map.
// Syntax is a single line per assertion, e.g.:
//   P, average(temperature,30 C).
//   Q, not(detected(water,500)).
// Lines starting with '#' are treated as comments.
bool MSLAssertionLoader::load(std::istream& in,
                              std::map<std::string, Proposition>& out) const {
    out.clear();

    std::string line;
    while (std::getline(in, line)) {
        std::string t = trim(line);
        if (t.empty()) continue;
        if (t[0] == '#') continue;

        // Drop trailing '.' if present.
        if (!t.empty() && t.back() == '.') {
            t.pop_back();
        }

        // Expect: Label, core
        std::size_t commaPos = t.find(',');
        if (commaPos == std::string::npos) {
            return false; // hard syntax error
        }

        std::string labelPart = trim(t.substr(0, commaPos));
        std::string corePart  = trim(t.substr(commaPos + 1));
        if (labelPart.empty() || corePart.empty()) return false;

        std::string label = labelPart;
        bool        negated = false;
        std::string core = corePart;

        // Detect an outer not(...) wrapper, e.g. not(detected(water,500)).
        std::string lowerCore = lowerNoSpace(corePart);
        if (lowerCore.rfind("not(", 0) == 0 && lowerCore.back() == ')') {
            negated = true;
            std::size_t openPos  = core.find('(');
            std::size_t closePos = core.find_last_of(')');
            if (openPos == std::string::npos ||
                closePos == std::string::npos || closePos <= openPos + 1) {
                return false;
            }
            core = trim(core.substr(openPos + 1, closePos - openPos - 1));
        }

        Proposition p = parseCore(core, label);
        p.setNegated(negated);

        // If the label repeats, the last one wins.
        out[label] = p;
    }

    return true;
}

// Parse the "core" part of an assertion, e.g. "detected(water,500)" or
// "average(temperature,30 C)", using label as the proposition name.
Proposition MSLAssertionLoader::parseCore(const std::string& core,
                                          const std::string& label) {
    std::string s = trim(core);
    std::size_t openPos  = s.find('(');
    std::size_t closePos = s.find_last_of(')');
    if (openPos == std::string::npos ||
        closePos == std::string::npos || closePos <= openPos + 1) {
        return Proposition(label,
                           PredicateForm::Unknown,
                           Subject::Unknown,
                           ParamKind::None,
                           0.0,
                           false);
    }

    std::string predName = trim(s.substr(0, openPos));
    std::string inner    = trim(s.substr(openPos + 1,
                                         closePos - openPos - 1));

    std::size_t commaPos = inner.find(',');
    if (commaPos == std::string::npos) {
        return Proposition(label,
                           PredicateForm::Unknown,
                           Subject::Unknown,
                           ParamKind::None,
                           0.0,
                           false);
    }

    std::string subjTok  = trim(inner.substr(0, commaPos));
    std::string paramTok = trim(inner.substr(commaPos + 1));

    PredicateForm form = Proposition::predicateFromString(predName);
    Subject       subj = Proposition::subjectFromString(subjTok);

    // Initialize with no parameter; parseParam will fill in kind/value.
    Proposition p(label,
                  form,
                  subj,
                  ParamKind::None,
                  0.0,
                  false);

    parseParam(p, paramTok);
    return p;
}

// Interpret the parameter part of an assertion and set kind/value on p.
// Handles temperature (°C vs K), distance, and a default scalar intensity.
void MSLAssertionLoader::parseParam(Proposition& p,
                                    const std::string& paramToken) {
    std::string t = lowerNoSpace(paramToken);
    if (t.empty()) {
        p.setParamKind(ParamKind::None);
        p.setValue(0.0);
        return;
    }

    // Split into numeric prefix and unit suffix, e.g. "30c" → "30", "c".
    std::size_t i = 0;
    if (t[0] == '+' || t[0] == '-') ++i;
    while (i < t.size() &&
           (std::isdigit(static_cast<unsigned char>(t[i])) || t[i] == '.')) {
        ++i;
    }

    std::string numPart  = t.substr(0, i);
    std::string unitPart = t.substr(i);

    double val = 0.0;
    try {
        val = std::stod(numPart);
    } catch (...) {
        val = 0.0;
    }

    Subject       subj = p.subject();
    PredicateForm form = p.form();

    // Temperature-style numbers:
    //   - predicate Average(...) always treated as temperature-like
    //   - or subject explicitly Temperature.
    if (subj == Subject::Temperature || form == PredicateForm::Average) {
        p.setParamKind(ParamKind::Temperature);
        if (unitPart == "c") {
            p.setValue(val + 273.15); // convert °C → K for storage
        } else {
            p.setValue(val);          // assume already in K
        }
        return;
    }

    // Distance-style numbers: store meters directly.
    if (subj == Subject::Distance) {
        p.setParamKind(ParamKind::Distance);
        p.setValue(val);
        return;
    }

    // Default: intensity-style scalar.
    p.setParamKind(ParamKind::Intensity);
    p.setValue(val);
}
