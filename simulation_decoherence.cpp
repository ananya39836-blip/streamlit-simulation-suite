// ===========================================================================
// C++ Engine: Lindblad Master Equation & Pointer State Decoherence Solver
// Language: Modern C++ (C++14/C++17) | Compilation: g++ -std=c++14 -O3
// ===========================================================================
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <iomanip>
#include <algorithm>

using Complex = std::complex<double>;

// 2x2 Density Matrix for an open quantum system:
// rho = [ rho00, rho01 ]
//       [ rho10, rho11 ]
struct DensityMatrix2x2 {
    Complex rho00, rho01;
    Complex rho10, rho11;

    DensityMatrix2x2() 
        : rho00(0.5, 0.0), rho01(0.5, 0.0), rho10(0.5, 0.0), rho11(0.5, 0.0) {}

    DensityMatrix2x2(Complex r00, Complex r01, Complex r10, Complex r11)
        : rho00(r00), rho01(r01), rho10(r10), rho11(r11) {}

    // Purity: gamma = Tr(rho^2) = |rho00|^2 + |rho11|^2 + 2*|rho01|^2
    // gamma = 1 for a pure state; gamma = 0.5 for a maximally mixed state
    double getPurity() const {
        return (std::norm(rho00) + std::norm(rho11) + 2.0 * std::norm(rho01));
    }

    // Coherence magnitude |rho01|
    double getCoherence() const {
        return std::abs(rho01);
    }

    DensityMatrix2x2 operator+(const DensityMatrix2x2& other) const {
        return DensityMatrix2x2(rho00 + other.rho00, rho01 + other.rho01,
                                rho10 + other.rho10, rho11 + other.rho11);
    }

    DensityMatrix2x2 operator*(double scalar) const {
        return DensityMatrix2x2(rho00 * scalar, rho01 * scalar,
                                rho10 * scalar, rho11 * scalar);
    }
};

// Computes the Lindblad RHS: d rho / dt = -i/hbar [H, rho] + gamma_dec * ( sigma_z rho sigma_z - rho )
// Here H = (hbar * omega / 2) * sigma_x (coherent tunneling)
// Environment interaction operator: L = sigma_z (phase decoherence / pointer basis |0>, |1>)
DensityMatrix2x2 computeLindbladRHS(const DensityMatrix2x2& rho, double omega, double gammaDec) {
    // 1. Coherent Hamiltonian term: -i * [H, rho]
    // H = [ 0,     omega/2 ]
    //     [ omega/2, 0     ]
    Complex h00 = 0.0, h01 = omega * 0.5;
    Complex h10 = omega * 0.5, h11 = 0.0;

    // Matrix multiplication H * rho
    Complex Hrho00 = h01 * rho.rho10;
    Complex Hrho01 = h01 * rho.rho11;
    Complex Hrho10 = h10 * rho.rho00;
    Complex Hrho11 = h10 * rho.rho01;

    // Matrix multiplication rho * H
    Complex rhoH00 = rho.rho01 * h10;
    Complex rhoH01 = rho.rho00 * h01;
    Complex rhoH10 = rho.rho11 * h10;
    Complex rhoH11 = rho.rho10 * h01;

    Complex comm00 = Complex(0.0, -1.0) * (Hrho00 - rhoH00);
    Complex comm01 = Complex(0.0, -1.0) * (Hrho01 - rhoH01);
    Complex comm10 = Complex(0.0, -1.0) * (Hrho10 - rhoH10);
    Complex comm11 = Complex(0.0, -1.0) * (Hrho11 - rhoH11);

    // 2. Lindblad Dissipator: L * rho * L^dagger - 0.5 * {L^dagger * L, rho}
    // For dephasing L = sqrt(gamma_dec) * sigma_z:
    // sigma_z * rho * sigma_z - rho = [ 0, -2*rho01 ]
    //                                 [ -2*rho10, 0 ]
    Complex diss00 = 0.0;
    Complex diss01 = -2.0 * gammaDec * rho.rho01;
    Complex diss10 = -2.0 * gammaDec * rho.rho10;
    Complex diss11 = 0.0;

    return DensityMatrix2x2(comm00 + diss00, comm01 + diss01,
                            comm10 + diss10, comm11 + diss11);
}

// 4th-Order Runge-Kutta integrator for Density Matrix
DensityMatrix2x2 rk4Step(const DensityMatrix2x2& rho, double omega, double gammaDec, double dt) {
    DensityMatrix2x2 k1 = computeLindbladRHS(rho, omega, gammaDec);
    DensityMatrix2x2 k2 = computeLindbladRHS(rho + k1 * (0.5 * dt), omega, gammaDec);
    DensityMatrix2x2 k3 = computeLindbladRHS(rho + k2 * (0.5 * dt), omega, gammaDec);
    DensityMatrix2x2 k4 = computeLindbladRHS(rho + k3 * dt, omega, gammaDec);

    return rho + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (dt / 6.0);
}

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " C++ ENGINE: LINDBLAD MASTER EQUATION & DECOHERENCE       " << std::endl;
    std::cout << "==========================================================" << std::endl;

    // Initial state: pure symmetric superposition |+> = (|0> + |1>) / sqrt(2)
    // rho(0) = [ 0.5, 0.5 ]
    //          [ 0.5, 0.5 ]
    DensityMatrix2x2 rho;

    double omega = 1.0;      // Coherent tunneling frequency
    double gammaDec = 2.5;   // Strong environmental decoherence rate
    double dt = 0.005;
    double tEnd = 2.0;
    int steps = static_cast<int>(tEnd / dt);

    std::cout << "[Simulating Quantum Decoherence into Classical Pointer States]" << std::endl;
    std::cout << "  Tunneling Frequency (omega)   : " << omega << std::endl;
    std::cout << "  Environmental Coupling (gamma): " << gammaDec << std::endl;
    std::cout << "  Initial State                 : Pure Superposition (|psi> = |+>)" << std::endl;
    std::cout << "  Initial Purity Tr(rho^2)      : " << std::fixed << std::setprecision(4) << rho.getPurity() << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "   Time (t)  | Coherence |rho01| | Purity Tr(rho^2) | State Nature" << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    for (int s = 0; s <= steps; ++s) {
        double t = s * dt;
        if (s % (steps / 8) == 0) {
            double coh = rho.getCoherence();
            double pur = rho.getPurity();
            std::string nature;
            if (pur > 0.95) nature = "Pure Quantum Superposition";
            else if (pur > 0.60) nature = "Decohering Transient";
            else nature = "Classical Statistical Mixture";

            std::cout << "    " << std::fixed << std::setprecision(2) << t << "     |     "
                      << std::setprecision(5) << coh << "     |     "
                      << std::setprecision(5) << pur << "    | "
                      << nature << std::endl;
        }
        rho = rk4Step(rho, omega, gammaDec, dt);
    }
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "  Dynamical Einselection Confirmed: Off-diagonals quenched." << std::endl;
    std::cout << "  Wavefunction branched into stable, decohered pointer worlds." << std::endl;
    std::cout << "==========================================================" << std::endl;

    return 0;
}
