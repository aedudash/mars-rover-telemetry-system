// MSLAssertionLoader.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Load MSL assertions into a map<string, Proposition>.

#pragma once

#include <iosfwd>
#include <map>
#include <string>

#include "Proposition.h"

class MSLAssertionLoader {
public:
    // PRE: 'in' is an open stream on msl_assertion.txt
    // POST: out is filled with label -> Proposition; returns false on hard syntax error
    bool load(std::istream& in,
              std::map<std::string, Proposition>& out) const;

private:
    static std::string trim(const std::string& s);
    static std::string lowerNoSpace(const std::string& s);

    // Parse "detected(water,500)" or "average(temperature,30c)" into a Proposition
    static Proposition parseCore(const std::string& core,
                                 const std::string& label);

    // Interpret numeric + unit-ish token into ParamKind + canonical value
    static void parseParam(Proposition& p, const std::string& paramToken);
};
