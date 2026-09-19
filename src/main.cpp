// main.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Run the rover + logic pipeline and write mission_report.txt

#include "Robot.h"
#include "MSLAssertionLoader.h"
#include "PerseveranceBuilder.h"
#include "TruthReconciler.h"
#include "Ratiocinator.h"
#include "Proposition.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

// ---------- printing helpers ----------

// Basic usage message for command-line invocation.
static void printUsage(const char* prog) {
    std::cerr
        << "Usage:\n  "
        << prog
        << " <msl_assertion.txt> <argument.txt> <data1> [data2 ...]\n";
}

// Turn a TruthValue enum into a readable string.
static const char* tvToString(TruthValue tv) {
    switch (tv) {
    case TruthValue::True:    return "True";
    case TruthValue::False:   return "False";
    case TruthValue::Unknown: return "Unknown";
    }
    return "Unknown";
}

static const char* predicateFormToString(PredicateForm pf) {
    switch (pf) {
    case PredicateForm::Detected: return "detected";
    case PredicateForm::All:      return "all";
    case PredicateForm::Some:     return "some";
    case PredicateForm::None:     return "no";
    case PredicateForm::Is:       return "is";
    case PredicateForm::Average:  return "average";
    case PredicateForm::Unknown:  default: return "unknown";
    }
}

static const char* subjectToString(Subject s) {
    switch (s) {
    case Subject::Water:         return "water";
    case Subject::Hydrogen:      return "hydrogen";
    case Subject::Temperature:   return "temperature";
    case Subject::Distance:      return "distance";
    case Subject::Life:          return "life";
    case Subject::MicrobialLife: return "microbial_life";
    case Subject::AncientLife:   return "ancient_life";
    case Subject::Unknown: default: return "unknown";
    }
}

static const char* paramKindToString(ParamKind pk) {
    switch (pk) {
    case ParamKind::None:        return "none";
    case ParamKind::Intensity:   return "intensity";
    case ParamKind::Distance:    return "distance";
    case ParamKind::Temperature: return "temperature";
    }
    return "none";
}

// Print a single Proposition in a compact, readable format.
// (Retained for reference; main now uses operator<< on Proposition directly.)
static void printProposition(std::ostream& os, const Proposition& p) {
    os << p;
}

// ---------- main driver ----------

int main(int argc, char** argv) {
    // argv[1] = MSL assertion file
    // argv[2] = argument file
    // argv[3..] = one or more rover data files
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }

    const std::string mslPath = argv[1];  // assertion file (any name)
    const std::string argPath = argv[2];  // argument file  (any name)

    std::ofstream out("mission_report.txt", std::ios::trunc);
    if (!out) {
        std::cerr << "Error: could not open mission_report.txt for writing.\n";
        return 1;
    }

    MSLAssertionLoader  mslLoader;
    PerseveranceBuilder perBuilder;
    TruthReconciler     reconciler;

    // Process each rover data file as its own report section.
    for (int i = 3; i < argc; ++i) {
        const std::string dataPath = argv[i];

        const std::size_t separator = dataPath.find_last_of("/\\");
	const std::string dataFileName =
    (separator == std::string::npos)
        ? dataPath
        : dataPath.substr(separator + 1);

	out << "============================================================\n";
	out << "Report for input file: " << dataFileName << "\n";
	out << "============================================================\n";

        // 1) Measurement layer: Robot (temps, samples, navigation).
        Robot r;
        r.loadFile(dataPath);
        r.run();
        r.writeReport(out);

        // 2) MSL propositions (P..T) from professor's assertion file.
        std::map<std::string, Proposition> mslProps;
        {
            std::ifstream msl(mslPath);
            if (!msl) {
                std::cerr << "Error: could not open MSL assertion file: "
                          << mslPath << "\n";
                return 1;
            }
            if (!mslLoader.load(msl, mslProps)) {
                std::cerr << "Error: failed to parse MSL assertion file: "
                          << mslPath << "\n";
                return 1;
            }
        }

        // 3) Perseverance propositions (P..T) derived from rover data.
        std::map<std::string, Proposition> perProps =
            perBuilder.buildFromRobot(r);

        // 4) Reconcile τ(P..T) between MSL and Perseverance.
        std::map<std::string, TruthValue> tau =
            reconciler.reconcile(mslProps, perProps);

        // 5) Ratiocinator: load arguments X, Y, Z using the reconciled τ.
        Ratiocinator rat;
        for (const auto& kv : tau) {
            rat.setTruth(kv.first, kv.second);
        }

        {
            std::ifstream arg(argPath);
            if (!arg) {
                std::cerr << "Error: could not open argument file: "
                          << argPath << "\n";
                return 1;
            }
            if (!rat.loadArguments(arg)) {
                std::cerr << "Error: failed to parse argument file: "
                          << argPath << "\n";
                return 1;
            }
        }

        rat.evaluateArguments();

        // 6) Logical sections in report.

        out << "\n-- Propositions P-T (MSL vs Perseverance) --\n";
        std::vector<std::string> labels = {"P","Q","R","S","T"};
        for (const auto& L : labels) {
            out << "Label " << L << ":\n";

            auto mit = mslProps.find(L);
            out << "  MSL: ";
            if (mit == mslProps.end()) {
                out << "<none>\n";
            } else {
                out << mit->second << "\n";
            }

            auto pit = perProps.find(L);
            out << "  PER: ";
            if (pit == perProps.end()) {
                out << "<none>\n";
            } else {
                out << pit->second << "\n";
            }

            auto tit = tau.find(L);
            TruthValue tv = (tit == tau.end())
                            ? TruthValue::Unknown
                            : tit->second;
            out << "  tau(" << L << ") = " << tv << "\n";
        }

        out << "\n-- Arguments X, Y, Z --\n";
        for (const auto& a : rat.arguments()) {
            out << a << "\n";
        }

        out << "\n\n";
    }

    std::cout << "Reports written to mission_report.txt\n";
    return 0;
}
