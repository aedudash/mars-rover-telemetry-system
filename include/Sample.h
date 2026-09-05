// Sample.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Spectral sample (wavelength + intensity + sol).

#pragma once

#include <string>
#include "Measurement.h"

class Sample {
private:
    Measurement wavelength_; // spectral line wavelength
    double      intensity_;  // raw intensity (dimensionless)
    int         sol_;        // encounter sol index

public:
    // PRE: wavelength is a valid Measurement; intensity >= 0
    // POST: Stores fields
    Sample(const Measurement& wavelength, double intensity, int sol)
        : wavelength_(wavelength), intensity_(intensity), sol_(sol) {}

    // PRE: none
    // POST: Returns a coarse substance key by wavelength band
    std::string classification_key() const;

    // PRE: none
    // POST: Returns sol index
    int sol() const { return sol_; }

    // PRE: none
    // POST: Returns stored wavelength (original units)
    const Measurement& wavelength() const { return wavelength_; }

    // PRE: none
    // POST: Returns wavelength expressed in base meters
    double wavelengthBaseMeters() const { return wavelength_.asBase().value(); }

    // PRE: none
    // POST: Returns raw intensity value
    double intensity() const { return intensity_; }
};
