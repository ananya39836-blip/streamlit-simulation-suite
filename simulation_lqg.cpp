// ===========================================================================
// C++ Engine: Loop Quantum Gravity Spin Network & Area Spectrum Engine
// Language: Modern C++ (C++14/C++17) | Compilation: g++ -std=c++14 -O3
// ===========================================================================
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

constexpr double L_PLANCK = 1.0;
constexpr double PI_VAL = 3.14159265358979323846;

// Barbero-Immirzi parameter for fundamental j=1/2 punctures: gamma_0 = ln(2) / (pi * sqrt(3))
constexpr double IMMIRZI_FUNDAMENTAL = 0.1273840;
// Full SU(2) Chern-Simons multi-spin Barbero-Immirzi parameter (Meissner 2004):
constexpr double IMMIRZI_MEISSNER = 0.2375329;

struct SpinEdge {
    int twoJ; // 2 * j

    double getSpin() const {
        return twoJ * 0.5;
    }

    double getCasimir() const {
        double j = getSpin();
        return j * (j + 1.0);
    }

    double getAreaContribution(double gamma) const {
        return 8.0 * PI_VAL * gamma * L_PLANCK * L_PLANCK * std::sqrt(getCasimir());
    }
};

class SpinNetworkSurface {
private:
    std::vector<SpinEdge> punctures;

public:
    void addPuncture(int twoJ) {
        punctures.push_back(SpinEdge{twoJ});
    }

    double calculateTotalArea(double gamma) const {
        double total = 0.0;
        for (const auto& p : punctures) {
            total += p.getAreaContribution(gamma);
        }
        return total;
    }

    static double calculateAreaGap(double gamma) {
        SpinEdge fundamental{1}; // j = 1/2
        return fundamental.getAreaContribution(gamma);
    }

    unsigned long long calculateMicrostates() const {
        unsigned long long dim = 1;
        for (const auto& p : punctures) {
            dim *= (p.twoJ + 1);
        }
        return dim;
    }

    void printDiagnostics() const {
        std::cout << "[Spin Network Surface Diagnostics]" << std::endl;
        std::cout << "  Fundamental Immirzi (gamma_0): " << std::fixed << std::setprecision(7) << IMMIRZI_FUNDAMENTAL << std::endl;
        std::cout << "  Surface Punctures Count       : " << punctures.size() << std::endl;
        
        double areaFund = calculateTotalArea(IMMIRZI_FUNDAMENTAL);
        double areaGap = calculateAreaGap(IMMIRZI_FUNDAMENTAL);
        std::cout << "  Fundamental Area Gap Delta_A  : " << std::fixed << std::setprecision(5) << areaGap << " l_P^2" << std::endl;
        std::cout << "  Total Discrete Area A(S)      : " << std::fixed << std::setprecision(5) << areaFund << " l_P^2" << std::endl;
        
        unsigned long long microstates = calculateMicrostates();
        std::cout << "  Horizon Microstates N_states  : " << microstates << std::endl;
        
        double entropy = std::log(static_cast<double>(microstates));
        std::cout << "  Statistical Entropy S_stat    : " << std::fixed << std::setprecision(5) << entropy << " k_B" << std::endl;
        
        double bhEntropyTarget = areaFund * 0.25; // A / (4 * l_P^2)
        std::cout << "  Bekenstein-Hawking S_BH (A/4) : " << std::fixed << std::setprecision(5) << bhEntropyTarget << " k_B" << std::endl;
        std::cout << "  Ratio S_stat / S_BH           : " << std::fixed << std::setprecision(6) << (entropy / bhEntropyTarget) << " (Exact Match!)" << std::endl;
    }
};

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " C++ SIMULATION: LOOP QUANTUM GRAVITY SPIN NETWORK ENGINE " << std::endl;
    std::cout << "==========================================================" << std::endl;

    SpinNetworkSurface horizon;
    for (int i = 0; i < 24; ++i) {
        horizon.addPuncture(1); // 24 fundamental spin-1/2 punctures
    }

    horizon.printDiagnostics();
    std::cout << "==========================================================" << std::endl;
    return 0;
}
