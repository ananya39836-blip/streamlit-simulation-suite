// ===========================================================================
// C++ Engine: Digital Physics Cellular Automaton & Glider Scattering Engine
// Language: Modern C++ (C++14/C++17) | Compilation: g++ -std=c++14 -O3
// ===========================================================================
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>
#include <bitset>

// Elementary Cellular Automaton Rule 110:
// 111 -> 0, 110 -> 1, 101 -> 1, 100 -> 0, 011 -> 1, 010 -> 1, 001 -> 1, 000 -> 0
// Binary: 01101110 = 110 in decimal (Turing Complete by Cook's Theorem, 2004)
constexpr uint8_t RULE_110 = 110;

class CellularUniverse {
private:
    int width;
    std::vector<uint8_t> grid;
    uint8_t rule;

public:
    CellularUniverse(int size, uint8_t ruleByte = RULE_110)
        : width(size), grid(size, 0), rule(ruleByte) {}

    void setCell(int index, uint8_t val) {
        if (index >= 0 && index < width) {
            grid[index] = val;
        }
    }

    // Step forward one discrete Planck time unit
    void step() {
        std::vector<uint8_t> nextGrid(width, 0);
        for (int i = 0; i < width; ++i) {
            int left = (i == 0) ? grid[width - 1] : grid[i - 1];
            int center = grid[i];
            int right = (i == width - 1) ? grid[0] : grid[i + 1];

            // Neighborhood configuration (0 to 7)
            int neighborhood = (left << 2) | (center << 1) | right;
            nextGrid[i] = (rule >> neighborhood) & 1;
        }
        grid = nextGrid;
    }

    // Shannon Information Entropy of spatial bitstring: H = - sum p_i * log2(p_i)
    double calculateSpatialEntropy() const {
        int ones = 0;
        for (uint8_t cell : grid) if (cell) ones++;
        if (ones == 0 || ones == width) return 0.0;

        double p1 = static_cast<double>(ones) / width;
        double p0 = 1.0 - p1;
        return -(p0 * std::log2(p0) + p1 * std::log2(p1));
    }

    // Population count (analog of total mass/energy)
    int getActiveCellCount() const {
        int count = 0;
        for (uint8_t cell : grid) if (cell) count++;
        return count;
    }

    // Displays a spatial snapshot
    void printSlice(int timeStep) const {
        std::cout << "t = " << std::setw(3) << timeStep << " | Pop = " 
                  << std::setw(3) << getActiveCellCount() 
                  << " | H = " << std::fixed << std::setprecision(4) << calculateSpatialEntropy() 
                  << " bits | ";
        for (int i = 0; i < std::min(width, 40); ++i) {
            std::cout << (grid[i] ? "#" : ".");
        }
        std::cout << "..." << std::endl;
    }
};

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " C++ ENGINE: DIGITAL PHYSICS & CELLULAR AUTOMATA (RULE 110) " << std::endl;
    std::cout << "==========================================================" << std::endl;

    const int UNIVERSE_WIDTH = 120;
    CellularUniverse universe(UNIVERSE_WIDTH, RULE_110);

    // Initialize with a localized persistent glider seed (analog of an elementary particle)
    universe.setCell(60, 1);
    universe.setCell(61, 1);
    universe.setCell(62, 1);
    universe.setCell(63, 0);
    universe.setCell(64, 1);

    std::cout << "[Simulating Emergent Computation & Particle Propagation]" << std::endl;
    std::cout << "  Rule Code       : 110 (Universal Turing Complete)" << std::endl;
    std::cout << "  Lattice Lattice : 120 Discrete Cells with Periodic Boundaries" << std::endl;
    std::cout << "  Active Pattern  : Soliton Glider Wavepacket" << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    for (int t = 0; t <= 16; ++t) {
        if (t % 2 == 0) {
            universe.printSlice(t);
        }
        universe.step();
    }

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "  Discrete light-cone speed: c_lattice = 1 cell / step." << std::endl;
    std::cout << "  Deterministic discrete rules generate universal physics!" << std::endl;
    std::cout << "==========================================================" << std::endl;

    return 0;
}
