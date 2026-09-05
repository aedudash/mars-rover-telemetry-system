// TruthReconciler.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Reconcile MSL and Perseverance propositions to τ(P..T).

#pragma once

#include <map>
#include <string>

#include "Proposition.h"
#include "Ratiocinator.h" // for TruthValue

class TruthReconciler {
public:
    // PRE: msl and per contain labeled propositions
    // POST: returns τ: label -> TruthValue
    std::map<std::string, TruthValue>
    reconcile(const std::map<std::string, Proposition>& msl,
              const std::map<std::string, Proposition>& per) const;
};
