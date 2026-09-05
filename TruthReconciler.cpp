// TruthReconciler.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Implementation of reconciliation τ(P..T).
//          Compares MSL and Perseverance propositions by label and
//          assigns a three-valued truth result for each label.

#include "TruthReconciler.h"

#include <set>

std::map<std::string, TruthValue>
TruthReconciler::reconcile(const std::map<std::string, Proposition>& msl,
                           const std::map<std::string, Proposition>& per) const {
    std::map<std::string, TruthValue> tau;

    // Collect the union of all labels seen in either map.
    std::set<std::string> labels;
    for (const auto& kv : msl) labels.insert(kv.first);
    for (const auto& kv : per) labels.insert(kv.first);

    // Walk through every label once and decide its reconciled truth.
    for (const auto& label : labels) {
        auto mIt = msl.find(label);
        auto pIt = per.find(label);

        bool haveM = (mIt != msl.end());
        bool haveP = (pIt != per.end());

        if (haveM && haveP) {
            // Both probes have something to say about this label.
            const Proposition& pm = mIt->second;
            const Proposition& pp = pIt->second;

            if (pm.equivalentTo(pp)) {
                // Same proposition structurally → we treat it as true.
                tau[label] = TruthValue::True;
            } else if (pm.isNegationOf(pp) || pp.isNegationOf(pm)) {
                // Direct contradiction between probes → unknown / conflict.
                tau[label] = TruthValue::Unknown;
            } else {
                // Mismatched structure (not equal or clean negation) →
                // we don't try to over-interpret it here, so mark unknown.
                tau[label] = TruthValue::Unknown;
            }
        } else if (haveP && !haveM) {
            // Only Perseverance makes a claim. Use its negation flag
            // to decide true vs false from the reference frame of τ.
            const Proposition& pp = pIt->second;
            tau[label] = pp.negated() ? TruthValue::False
                                      : TruthValue::True;
        } else if (haveM && !haveP) {
            // Only MSL has information; current model does not assign
            // a definite truth in this case, so leave it as unknown.
            tau[label] = TruthValue::Unknown;
        } else {
            // Shouldn't really happen (label came from somewhere),
            // but default to unknown for completeness.
            tau[label] = TruthValue::Unknown;
        }
    }

    return tau;
}
