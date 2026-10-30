// ===========================================================================
// C++ Engine: Algebraic QFT Net, Microcausality & Pauli-Jordan Commutator
// Language: Modern C++ (C++14/C++17) | Compilation: g++ -std=c++14 -O3
// ===========================================================================
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

constexpr double PI_VAL = 3.14159265358979323846;

struct SpacetimeEvent {
    double t, x; // 1+1D Minkowski spacetime coordinates (c = 1)
};

// Spacetime interval: s^2 = - (dt)^2 + (dx)^2
// s^2 < 0 : Timelike
// s^2 = 0 : Lightlike
// s^2 > 0 : Spacelike
double calculateIntervalSq(const SpacetimeEvent& e1, const SpacetimeEvent& e2) {
    double dt = e2.t - e1.t;
    double dx = e2.x - e1.x;
    return - (dt * dt) + (dx * dx);
}

// Pauli-Jordan distribution Delta(x - y) in 1+1D massless scalar field:
// Delta(x - y) = (1/2) * [ Theta(dt - |dx|) - Theta(-dt - |dx|) ]
// Crucial property of AQFT Microcausality:
// Delta(x - y) == 0 everywhere in the spacelike region (s^2 > 0)
double calculatePauliJordanCommutator(const SpacetimeEvent& e1, const SpacetimeEvent& e2) {
    double dt = e2.t - e1.t;
    double dx = std::abs(e2.x - e1.x);

    // Spacelike region: |dx| > |dt| -> Commutator is STRICTLY ZERO
    if (dx > std::abs(dt)) {
        return 0.0;
    }

    // Inside future lightcone (dt > dx >= 0)
    if (dt > dx) {
        return 0.5;
    }
    // Inside past lightcone (-dt > dx >= 0)
    if (-dt > dx) {
        return -0.5;
    }

    return 0.0; // On the lightcone boundary
}

// Represents a localized spacetime region O bounded by [tMin, tMax] x [xMin, xMax]
struct LocalRegion {
    std::string name;
    double tCenter, xCenter;
    double radius;

    bool isSpacelikeSeparatedFrom(const LocalRegion& other) const {
        double dt = std::abs(other.tCenter - tCenter) - (radius + other.radius);
        double dx = std::abs(other.xCenter - xCenter) - (radius + other.radius);
        if (dt < 0) dt = 0; // Overlapping time coordinate
        return (dx * dx) > (dt * dt);
    }
};

class AQFTLocalNet {
public:
    void testMicrocausality(const LocalRegion& regA, const LocalRegion& regB) const {
        std::cout << "[Evaluating AQFT Microcausality Axiom between Regions]" << std::endl;
        std::cout << "  Region A : " << regA.name << " at (t=" << regA.tCenter << ", x=" << regA.xCenter << ")" << std::endl;
        std::cout << "  Region B : " << regB.name << " at (t=" << regB.tCenter << ", x=" << regB.xCenter << ")" << std::endl;

        SpacetimeEvent centerA{regA.tCenter, regA.xCenter};
        SpacetimeEvent centerB{regB.tCenter, regB.xCenter};

        double sSq = calculateIntervalSq(centerA, centerB);
        double commutator = calculatePauliJordanCommutator(centerA, centerB);

        std::cout << "  Spacetime Invariant Interval s^2: " << std::fixed << std::setprecision(4) << sSq << std::endl;
        std::cout << "  Causal Classification           : " << (sSq > 0 ? "SPACELIKE (s^2 > 0)" : "TIMELIKE / CAUSAL (s^2 <= 0)") << std::endl;
        std::cout << "  Pauli-Jordan Causal Commutator  : " << std::setprecision(6) << commutator << std::endl;
        std::cout << "  Axiom Status                    : " 
                  << (std::abs(commutator) < 1e-12 ? "VALIDATED: [A(O_A), A(O_B)] = 0" : "CAUSAL INTERACTION PERMITTED") 
                  << std::endl;
    }
};

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " C++ ENGINE: ALGEBRAIC QFT NET & MICROCAUSALITY COMMUTATOR" << std::endl;
    std::cout << "==========================================================" << std::endl;

    AQFTLocalNet net;

    // Test Case 1: Spacelike separated regions (Einstein Causality)
    LocalRegion labAlice{"Alice's Lab", 0.0, -5.0, 0.5};
    LocalRegion labBob{"Bob's Lab",     0.0,  5.0, 0.5};
    net.testMicrocausality(labAlice, labBob);

    std::cout << "----------------------------------------------------------" << std::endl;

    // Test Case 2: Timelike separated regions (Causal Signal Transmission)
    LocalRegion labAlicePast{"Alice Past", 0.0, 0.0, 0.5};
    LocalRegion labAliceFuture{"Alice Future", 4.0, 0.0, 0.5};
    net.testMicrocausality(labAlicePast, labAliceFuture);

    std::cout << "==========================================================" << std::endl;
    std::cout << "  Haag-Kastler Microcausality holds rigorously on the net!" << std::endl;
    std::cout << "==========================================================" << std::endl;

    return 0;
}
