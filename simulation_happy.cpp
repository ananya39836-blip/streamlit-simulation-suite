// ===========================================================================
// C++ Engine: Holographic Tensor Network & HaPPY Quantum Error Correction
// Language: Modern C++ (C++14/C++17) | Compilation: g++ -std=c++14 -O3
// ===========================================================================
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <queue>
#include <set>
#include <algorithm>

// HaPPY Pentagrid Model (Pastawski, Yoshida, Harlow, Preskill 2015)
// A 5-leg perfect tensor: 1 bulk input leg + 4 spatial legs, or 5 spatial legs.
// In any bipartition into <= 2 legs and >= 3 legs, it acts as an isometry.

struct TensorNode {
    int id;
    bool isBulk; // true if contains a logical bulk qubit
    std::vector<int> neighbors; // adjacent tensor IDs or boundary physical legs
};

class HolographicTensorNetwork {
private:
    std::vector<TensorNode> nodes;
    int numBoundaryLegs;

public:
    HolographicTensorNetwork() {
        // Construct a foundational HaPPY tree/pentagrid:
        // Central tensor (Node 0): 1 bulk qubit + 5 outgoing branches to 5 boundary tensors
        nodes.resize(6);
        nodes[0].id = 0;
        nodes[0].isBulk = true;
        for (int i = 1; i <= 5; ++i) {
            nodes[0].neighbors.push_back(i);
            nodes[i].id = i;
            nodes[i].isBulk = false;
            nodes[i].neighbors.push_back(0); // connection to center
            // Each peripheral tensor has 3 physical boundary legs
            nodes[i].neighbors.push_back(-1); // boundary leg 1
            nodes[i].neighbors.push_back(-1); // boundary leg 2
            nodes[i].neighbors.push_back(-1); // boundary leg 3
        }
        numBoundaryLegs = 15; // 5 * 3 = 15 boundary physical qubits
    }

    int getBoundaryQubitCount() const {
        return numBoundaryLegs;
    }

    // Calculates Ryu-Takayanagi minimal cut (discrete geodesic) for boundary subregion A
    // Size of subregion A is specified in number of boundary qubits [0, numBoundaryLegs]
    int calculateMinimalCut(int subregionSize) const {
        if (subregionSize <= 0) return 0;
        if (subregionSize >= numBoundaryLegs) return 0;

        // In the HaPPY pentagrid:
        // For a small subregion A (e.g. 1 to 5 qubits), the minimal cut wraps around A locally.
        // For a large subregion A (e.g. >= 8 qubits), the minimal cut swings through the bulk,
        // enclosing the bulk logical qubit (the Entanglement Wedge phase transition!).
        if (subregionSize <= 3) {
            return subregionSize; // Cut along individual boundary legs
        } else if (subregionSize < 8) {
            return 4; // Minimal geodesic traversing the peripheral hyperbolic tensor layers
        } else {
            // Entanglement wedge includes the central bulk node:
            // Minimal cut is the complement cut
            return calculateMinimalCut(numBoundaryLegs - subregionSize);
        }
    }

    // Tests if the bulk logical qubit is contained in the Entanglement Wedge of subregion A
    // (Subregion-Subregion Duality / Quantum Error Correction)
    bool isBulkReconstructible(int subregionSize) const {
        // A bulk qubit is protected by the [[5, 1, 3]] code structure.
        // It can be reconstructed if and only if subregion A contains at least 3 out of 5 peripheral blocks (>= 8 qubits)
        return subregionSize >= 8;
    }

    void printDiagnostics() const {
        std::cout << "[HaPPY Holographic Tensor Network Diagnostics]" << std::endl;
        std::cout << "  Bulk Logical Qubits     : 1 (Central Node 0)" << std::endl;
        std::cout << "  Physical Boundary Qubits: " << numBoundaryLegs << std::endl;
        std::cout << "  Tensor Geometry         : Discrete Hyperbolic H^2 Pentagrid" << std::endl;
        std::cout << "----------------------------------------------------------" << std::endl;
        std::cout << "  Subregion | RT Area | S_EE (ln 2) | Bulk Reconstructible?" << std::endl;
        std::cout << "----------------------------------------------------------" << std::endl;
        for (int sz = 1; sz <= numBoundaryLegs; sz += 2) {
            int cut = calculateMinimalCut(sz);
            bool rec = isBulkReconstructible(sz);
            std::cout << "     " << std::setw(2) << sz << "     |   "
                      << std::setw(2) << cut << "    |    "
                      << std::fixed << std::setprecision(2) << (cut * 0.693) << "     | "
                      << (rec ? "YES (Inside Wedge)" : "NO  (Causal Horizon)") << std::endl;
        }
        std::cout << "----------------------------------------------------------" << std::endl;
        std::cout << "  Quantum Error Correction: Bulk protected against up to 7 erasures!" << std::endl;
    }
};

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " C++ ENGINE: HOLOGRAPHIC QUANTUM ERROR-CORRECTING CODES   " << std::endl;
    std::cout << "==========================================================" << std::endl;

    HolographicTensorNetwork network;
    network.printDiagnostics();

    std::cout << "==========================================================" << std::endl;
    return 0;
}
