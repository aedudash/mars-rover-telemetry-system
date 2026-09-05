// SherlocClassifier.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Implementation for SHERLOC key aggregator with SOL tracking.
//          Tracks the first SOL where each unique classification key appears
//          and provides chronological and reversed views for reporting.

#include "SherlocClassifier.h"
#include <algorithm>

// Record the first SOL where a given classification key shows up.
// If we've already seen this key, ignore the new sample.
void SherlocClassifier::ingest(const Sample& s) {
    const std::string key = s.classification_key();
    // seen_.insert(key).second is true only if this key was newly inserted.
    if (seen_.insert(key).second) {
        results_.push_back({key, s.sol()});
    }
}

// Return (key, sol) pairs in original encounter order.
std::vector<std::pair<std::string,int>> SherlocClassifier::resultsChronological() const {
    return std::vector<std::pair<std::string,int>>(results_.begin(), results_.end());
}

// Return (key, sol) pairs in reverse encounter order, capped at 9 entries.
// The original encounter order is preserved inside results_.
std::vector<std::pair<std::string,int>> SherlocClassifier::resultsReverse() const {
    std::vector<std::pair<std::string,int>> r(results_.begin(), results_.end());
    std::reverse(r.begin(), r.end());
    if (r.size() > 9) r.resize(9);
    return r;
}
