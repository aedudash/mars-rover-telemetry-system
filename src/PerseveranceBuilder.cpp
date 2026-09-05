// PerseveranceBuilder.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 3
// Purpose: Implementation of Perseverance proposition construction.
//          Derives P..T from the robot’s temperature, spectral, and nav data.

#include "PerseveranceBuilder.h"

#include <cmath>

#include "Robot.h"

std::map<std::string, Proposition>
PerseveranceBuilder::buildFromRobot(const Robot& robot) const {
    std::map<std::string, Proposition> per;

    // --- 1) Temperature summary (P and T) ---
    const auto& temps = robot.temps();
    double meanK = temps.meanK();
    double meanC = meanK - 273.15;  // work in °C for comparisons

    // --- 2) Spectral intensities (Q, R) ---
    const auto& samples = robot.samples();
    double maxIntensity = 0.0;
    for (const auto& s : samples) {
        if (s.intensity() > maxIntensity) {
            maxIntensity = s.intensity();
        }
    }

    // --- 3) Navigation distance (S) ---
    const auto& days = robot.nav().days();
    double totalDistM = 0.0;
    for (const auto& d : days) {
        totalDistM += d.magnitude.asBase().value(); // meters
    }

    // Thresholds aligned with the earlier design for P..T.
    const double Q_THRESHOLD_INT   = 500.0;
    const double R_THRESHOLD_INT   = 1000.0;
    const double S_THRESHOLD_DIST  = 100.0;   // meters
    const double P_TARGET_C        = 30.0;    // °C
    const double T_TARGET_C        = -25.0;   // °C

    bool qDetected     = (maxIntensity >= Q_THRESHOLD_INT);
    bool rStrong       = (maxIntensity >= R_THRESHOLD_INT);
    bool sOver100      = (totalDistM   >= S_THRESHOLD_DIST);
    bool pAvg30        = (std::fabs(meanC - P_TARGET_C) <= 1e-3);
    bool tAvgMinus25   = (std::fabs(meanC - T_TARGET_C) <= 1e-3);

    // --- P: average(temperature,30c) ---
    {
        PredicateForm form = PredicateForm::Average;
        Subject       subj = Subject::Temperature;
        ParamKind     pk   = ParamKind::Temperature;
        double        valK = P_TARGET_C + 273.15;  // store in K

        // Negated means "average temperature is not 30°C".
        Proposition p("P", form, subj, pk, valK,
                      /*negated=*/!pAvg30);
        per["P"] = p;
    }

    // --- Q: detected(water,500) ---
    {
        PredicateForm form = PredicateForm::Detected;
        Subject       subj = Subject::Water;
        ParamKind     pk   = ParamKind::Intensity;
        double        val  = Q_THRESHOLD_INT;

        // Negated means "water NOT detected at or above 500 intensity".
        Proposition q("Q", form, subj, pk, val,
                      /*negated=*/!qDetected);
        per["Q"] = q;
    }

    // --- R: all(hydrogen,1000) ---
    {
        PredicateForm form = PredicateForm::All;
        Subject       subj = Subject::Hydrogen;
        ParamKind     pk   = ParamKind::Intensity;
        double        val  = R_THRESHOLD_INT;

        // Negated means "NOT all hydrogen readings reach 1000 intensity".
        Proposition r("R", form, subj, pk, val,
                      /*negated=*/!rStrong);
        per["R"] = r;
    }

    // --- S: detected(water,100m) ---
    {
        PredicateForm form = PredicateForm::Detected;
        Subject       subj = Subject::Water;
        ParamKind     pk   = ParamKind::Distance;
        double        val  = S_THRESHOLD_DIST;

        // Negated means "total water-related travel NOT at least 100 m".
        Proposition s("S", form, subj, pk, val,
                      /*negated=*/!sOver100);
        per["S"] = s;
    }

    // --- T: average(temperature,-25c) ---
    {
        PredicateForm form = PredicateForm::Average;
        Subject       subj = Subject::Temperature;
        ParamKind     pk   = ParamKind::Temperature;
        double        valK = T_TARGET_C + 273.15;  // store in K

        // Negated means "average temperature is not -25°C".
        Proposition t("T", form, subj, pk, valK,
                      /*negated=*/!tAvgMinus25);
        per["T"] = t;
    }

    return per;
}
