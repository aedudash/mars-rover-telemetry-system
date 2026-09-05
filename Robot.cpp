// Robot.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: File-driven integration of nav, temps, and SHERLOC outputs (CSV-style lines).
//          Reads a rover log file line-by-line, routes each record to the
//          appropriate subsystem, and emits a consolidated text report.

#include "Robot.h"
#include "Unit.h"
#include "Sample.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cmath>
#include <algorithm>

// Normalize a token: trim leading/trailing whitespace and lowercase it.
// Used mainly for the command words and directions.
static std::string norm(std::string s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    s = (a == std::string::npos) ? std::string() : s.substr(a, b - a + 1);
    for (auto& c : s) c = (char)std::tolower((unsigned char)(c));
    return s;
}

// Very simple CSV splitter (no quoting rules).
// Splits on ',', trims whitespace around each field, and returns a token list.
static std::vector<std::string> splitCSV(const std::string& line) {
    std::vector<std::string> out;
    std::string cur;
    std::istringstream is(line);
    while (std::getline(is, cur, ',')) {
        size_t a = cur.find_first_not_of(" \t\r\n");
        size_t b = cur.find_last_not_of(" \t\r\n");
        out.push_back(a == std::string::npos ? std::string() : cur.substr(a, b - a + 1));
    }
    return out;
}

// Build a Measurement object from a numeric string and a unit token.
// Uses the Unit registry to normalize units first.
static Measurement mFrom(const std::string& val, const std::string& unitTok) {
    Unit::registerDefaultUnits();
    Unit u = Unit::fromString(unitTok);
    return Measurement(std::stod(val), u);
}

// Map a direction token into a Dir enum, defaulting to Forward for unknowns.
static Dir dirFrom(const std::string& tok) {
    const std::string t = norm(tok);
    if (t == "left")     return Left;
    if (t == "right")    return Right;
    if (t == "backward") return Backward;
    return Forward;
}

// Main log-loader: walks the CSV-style file and dispatches each record
// into temps_, nav_, or sherloc_ based on the command token.
void Robot::loadFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) return;

    int  currentSol   = 0;     // integer index we pass to temperature + sample logic
    bool inSol        = false; // whether nav_ is currently inside a SOL block
    bool explicitMode = false; // true once we see an explicit "sol"/"end" pair

    std::string line;
    while (std::getline(in, line)) {
        // Skip blank lines and comment lines starting with '#'.
        std::string trimmed = line;
        size_t a = trimmed.find_first_not_of(" \t\r\n");
        if (a == std::string::npos) continue;
        if (trimmed[a] == '#') continue;

        auto t = splitCSV(line);
        if (t.empty()) continue;

        const std::string cmd = norm(t[0]);

        // Explicit SOL markers: "sol" starts a new navigation day, "end" closes it.
        if (cmd == "sol") {
            if (inSol) { nav_.endSOL(); ++currentSol; }
            nav_.beginSOL();
            inSol = true;
            explicitMode = true;
            continue;
        }

        if (cmd == "end") {
            if (inSol) { nav_.endSOL(); ++currentSol; inSol = false; }
            explicitMode = true;
            continue;
        }

        // Temperature record: "t,<value>,<unit>"
        if (cmd == "t") {
            if (t.size() >= 3) {
                try {
                    temps_.ingest(mFrom(t[1], t[2]), currentSol);
                } catch (...) {
                    // Bad numeric or unit; ignore this line.
                }
            }
            continue;
        }

        // Spectral sample: "w,<wavelength>,<unit>,<intensity?>"
        if (cmd == "w") {
            if (t.size() >= 3) {
                try {
                    Measurement wav = mFrom(t[1], t[2]);
                    double intensity = (t.size() >= 4) ? std::stod(t[3]) : 0.0;
                    Sample s(wav, intensity, currentSol);
                    samples_.push_back(s);
                    sherloc_.ingest(s);
                } catch (...) {
                    // If parsing fails, just drop this sample.
                }
            }
            continue;
        }

        // Displacement line: "d,<dist>,<unit>,<dir>,<time>,<timeUnit>,..."
        // Each movement consists of (distance, unit, direction, time, time-unit).
        // This supports both "implicit" and "explicit" SOL modes:
        //   - implicitMode (explicitMode == false):
        //       Each displacement line opens/closes its own SOL segment.
        //   - explicitMode (after a "sol" or "end"):
        //       Caller controls SOL boundaries directly, and we just add moves.
        if (cmd == "d") {
            bool openedHere = false;
            if (!explicitMode) {
                // Implicit mode: automatically open a SOL if we weren't in one.
                if (!inSol) { nav_.beginSOL(); inSol = true; openedHere = true; }
            } else {
                // Explicit mode: SOLs are already managed by "sol"/"end".
                if (!inSol) { nav_.beginSOL(); inSol = true; }
            }

            // Consume quintuplets of (distance, unit, direction, time, time-unit) across the line.
            for (std::size_t i = 1; i + 4 < t.size(); i += 5) {
                try {
                    Measurement dist = mFrom(t[i+0], t[i+1]);
                    Dir         d    = dirFrom(t[i+2]);
                    Measurement time = mFrom(t[i+3], t[i+4]);
                    nav_.addMove(dist, d, time);
                } catch (...) {
                    // Ignore any bad group and move on.
                }
            }

            // In implicit mode, close the SOL we opened just for this line.
            if (!explicitMode && openedHere) {
                nav_.endSOL(); ++currentSol; inSol = false;
            }
            continue;
        }
    }

    // If a SOL is still open at EOF, close it cleanly.
    if (inSol) { nav_.endSOL(); }
}

// Thin wrapper: give temps_ a chance to finalize any aggregates.
void Robot::run() {
    temps_.finalize();
}

// Emit the consolidated text report: temperature stats, SHERLOC summary,
// and navigation per-SOL distances/headings plus 100-SOL block statistics.
void Robot::writeReport(std::ostream& os) const {
    os << std::fixed << std::setprecision(3);

    // --- Temperature section ---
    os << "\n-- Temperature --\n";
    os << "mean C: "   << temps_.meanC()   << "\n";
    os << "mean K: "   << temps_.meanK()   << "\n";
    os << "median C: " << temps_.medianC() << "\n";
    os << "median K: " << temps_.medianK() << "\n";

    os << "top3 K: ";
    for (double v : temps_.top3K()) os << v << " ";
    os << "\n";

    os << "bottom3 K: ";
    for (double v : temps_.bottom3K()) os << v << " ";
    os << "\n";

    double summerK = temps_.lowestSummerK();
    os << "lowest Summer K: ";
    if (std::isinf(summerK)) os << "none\n";
    else                     os << summerK << "\n";

    double winterK = temps_.lowestWinterK();
    os << "lowest Winter K: ";
    if (std::isinf(winterK)) os << "none\n";
    else                     os << winterK << "\n";

    // --- SHERLOC / substances section ---
    os << "\n-- Unique substances by wavelength (chronological) --\n";
    for (const auto& kv : sherloc_.resultsChronological()) {
        os << kv.first << " (SOL " << kv.second << ")\n";
    }

    // --- Navigation section ---
    os << "\n-- Navigation --\n";
    int sol = 0;
    const auto& days = nav_.days();
    for (const auto& d : days) {
        os << "SOL " << sol++
           << "  distance=" << d.magnitude.toString()
           << "  heading="  << d.theta_deg << " deg"
           << "  time="     << d.total_time.toString()
           << "\n";
    }

    // --- 100-SOL block maximum distances (sorted descending) ---
    const std::size_t blockSize = 100;
    std::vector<Measurement> blockMax;
    for (std::size_t start = 0; start < days.size(); start += blockSize) {
        std::size_t end = std::min(start + blockSize, days.size());
        bool first = true;
        Measurement maxMag;
        for (std::size_t i = start; i < end; ++i) {
            const Measurement& mag = days[i].magnitude;
            if (first || mag.asBase().value() > maxMag.asBase().value()) {
                maxMag = mag;
                first = false;
            }
        }
        if (!first) blockMax.push_back(maxMag);
    }

    std::sort(blockMax.begin(), blockMax.end(),
              [](const Measurement& a, const Measurement& b) {
                  return a.asBase().value() > b.asBase().value(); // descending
              });

    os << "\n-- 100-SOL block max distances (sorted descending) --\n";
    for (const auto& m : blockMax) {
        os << m.toString() << "\n";
    }
}
