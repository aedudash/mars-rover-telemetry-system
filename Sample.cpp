// Sample.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Classification helper for spectral samples.
//          Maps wavelength into a compact substance key.

#include "Sample.h"

// Return a substance key, e.g. "Substance-UV", "Substance-VIS", "Substance-IR".
// Substance is chosen from wavelength in meters.
std::string Sample::classification_key() const {
    const double lambda_m = wavelength_.asBase().value(); // base meters

    // Spectral "substance" band based on wavelength.
    // Rough ranges:
    //   UV  : < 400 nm
    //   VIS : 400–700 nm
    //   IR  : > 700 nm
    std::string substance;
    if (lambda_m < 400e-9)        substance = "Substance-UV";
    else if (lambda_m <= 700e-9)  substance = "Substance-VIS";
    else                          substance = "Substance-IR";

    return substance;
}
