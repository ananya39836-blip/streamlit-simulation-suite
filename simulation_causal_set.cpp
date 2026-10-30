// ===========================================================================
// High-Performance C++ Engine: Discrete Causal Set & Dimension Estimator
// Language: Modern C++ (C++14/C++17) | Compilation: g++ -std=c++14 -O3
// ===========================================================================
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <iomanip>

struct SpacetimePoint {
    double t, x, y, z;
};

// Returns true if point A causally precedes point B (A < B) in Minkowski spacetime
bool causallyPrecedes(const SpacetimePoint& a, const SpacetimePoint& b) {
    double dt = b.t - a.t;
    if (dt <= 0.0) return false;
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double dz = b.z - a.z;
    double spatialDistSq = dx*dx + dy*dy + dz*dz;
    return (dt * dt) > spatialDistSq;
}

class CausalSetUniverse {
private:
    std::vector<SpacetimePoint> elements;
    int N;

public:
    CausalSetUniverse(int count) : N(count) {
        elements.reserve(N);
    }

    // Sprinkle points into an Alexandrov causal diamond between (0,0,0,0) and (2,0,0,0)
    void sprinklePoisson(unsigned int seed = 42) {
        std::mt19937_64 rng(seed);
        std::uniform_real_distribution<double> distT(0.0, 2.0);
        std::uniform_real_distribution<double> distSpatial(-1.0, 1.0);

        SpacetimePoint origin{0.0, 0.0, 0.0, 0.0};
        SpacetimePoint apex{2.0, 0.0, 0.0, 0.0};

        while (static_cast<int>(elements.size()) < N) {
            SpacetimePoint p{distT(rng), distSpatial(rng), distSpatial(rng), distSpatial(rng)};
            if (causallyPrecedes(origin, p) && causallyPrecedes(p, apex)) {
                elements.push_back(p);
            }
        }
    }

    // Meyer's Theorem (1988):
    // For N elements sprinkled uniformly in a d-dimensional causal diamond,
    // the fraction of pairs that are causally related converges to:
    // f(d) = Gamma(d+1) * Gamma(d/2) / (2 * Gamma(3d/2))
    double calculateEmergentDimension() const {
        long long relatedPairs = 0;
        long long totalPossiblePairs = static_cast<long long>(N) * (N - 1) / 2;

        for (int i = 0; i < N; ++i) {
            for (int j = i + 1; j < N; ++j) {
                if (causallyPrecedes(elements[i], elements[j]) || causallyPrecedes(elements[j], elements[i])) {
                    relatedPairs++;
                }
            }
        }

        double orderingFraction = static_cast<double>(relatedPairs) / static_cast<double>(totalPossiblePairs);

        // Invert f(d) using bisection numerical search
        // For d = 2, f(2) = 0.50. For d = 4, f(4) = 0.10. f(d) is strictly monotonic decreasing in d.
        double low = 1.0, high = 10.0, mid = 4.0;
        for (int iter = 0; iter < 100; ++iter) {
            mid = 0.5 * (low + high);
            double f_mid = (std::tgamma(mid + 1.0) * std::tgamma(mid * 0.5)) / (2.0 * std::tgamma(1.5 * mid));
            if (f_mid > orderingFraction) {
                low = mid; // f(d) is decreasing, so larger d gives smaller f
            } else {
                high = mid;
            }
        }
        return mid;
    }

    void printDiagnostics() const {
        std::cout << "[Causal Universe Diagnostics]" << std::endl;
        std::cout << "  Sprinkled Elements (N) : " << N << std::endl;
        double dim = calculateEmergentDimension();
        std::cout << "  Emergent Spacetime Dim : " << std::fixed << std::setprecision(4) << dim << std::endl;
        std::cout << "  Expected Target Dim    : 4.0000 (Minkowski 4D)" << std::endl;
        std::cout << "  Discreteness Residual  : " << std::abs(dim - 4.0) / 4.0 * 100.0 << " %" << std::endl;
    }
};

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " C++ SIMULATION: DISCRETE CAUSAL SET UNIVERSE & DIMENSION " << std::endl;
    std::cout << "==========================================================" << std::endl;
    CausalSetUniverse universe(1200);
    universe.sprinklePoisson(2026);
    universe.printDiagnostics();
    std::cout << "==========================================================" << std::endl;
    return 0;
}
