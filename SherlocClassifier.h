// SherlocClassifier.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Collects unique sample classifications with encounter SOL.

#pragma once

#include <set>
#include <string>
#include <vector>
#include "Sample.h"

class SherlocClassifier {
private:
    std::set<std::string> seen_;
    std::vector<std::pair<std::string,int>> results_; // {key, sol}

public:
    // PRE: none
    // POST: Ingests sample; records first-sol for each classification key
    void ingest(const Sample& s);

    // PRE: none
    // POST: Returns all results in chronological encounter order
    std::vector<std::pair<std::string,int>> resultsChronological() const;

    // PRE: none
    // POST: Returns up to 9 results in reverse encounter order
    std::vector<std::pair<std::string,int>> resultsReverse() const; // capped at 9
};
