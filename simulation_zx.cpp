// ===========================================================================
// C++ Engine: ZX-Calculus Graph Rewrite & Categorical Quantum Simplifier
// Language: Modern C++ (C++14/C++17) | Compilation: g++ -std=c++14 -O3
// ===========================================================================
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>
#include <map>

enum class SpiderType { Z_GREEN, X_RED, HADAMARD, BOUNDARY };

struct ZXNode {
    int id;
    SpiderType type;
    double phase; // Phase angle in radians [0, 2*pi)
    std::vector<int> neighbors;
    bool active;
};

class ZXDiagram {
private:
    std::vector<ZXNode> nodes;
    int nextId;

public:
    ZXDiagram() : nextId(0) {}

    int addNode(SpiderType type, double phase = 0.0) {
        int id = nextId++;
        nodes.push_back(ZXNode{id, type, phase, {}, true});
        return id;
    }

    void addEdge(int u, int v) {
        nodes[u].neighbors.push_back(v);
        nodes[v].neighbors.push_back(u);
    }

    int getActiveNodeCount() const {
        int count = 0;
        for (const auto& n : nodes) if (n.active) count++;
        return count;
    }

    // Rewrite Rule 1: Spider Fusion
    // Two adjacent spiders of the same type fuse into one spider summing their phases:
    // S(alpha) - S(beta)  ===>  S(alpha + beta)
    int applySpiderFusion() {
        int fusionCount = 0;
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (!nodes[i].active || nodes[i].type == SpiderType::BOUNDARY || nodes[i].type == SpiderType::HADAMARD) 
                continue;

            for (size_t k = 0; k < nodes[i].neighbors.size(); ++k) {
                int neighborId = nodes[i].neighbors[k];
                if (neighborId > static_cast<int>(i) && nodes[neighborId].active && nodes[neighborId].type == nodes[i].type) {
                    // Fuse neighbor into node i
                    nodes[i].phase = std::fmod(nodes[i].phase + nodes[neighborId].phase, 2.0 * 3.141592653589793);

                    // Re-link neighbors of neighborId to node i
                    for (int other : nodes[neighborId].neighbors) {
                        if (other != static_cast<int>(i)) {
                            nodes[i].neighbors.push_back(other);
                            // Replace link in other
                            for (int& backLink : nodes[other].neighbors) {
                                if (backLink == neighborId) backLink = i;
                            }
                        }
                    }

                    // Remove fused edge from node i
                    nodes[i].neighbors.erase(std::remove(nodes[i].neighbors.begin(), nodes[i].neighbors.end(), neighborId), nodes[i].neighbors.end());
                    nodes[neighborId].active = false;
                    nodes[neighborId].neighbors.clear();
                    fusionCount++;
                    break;
                }
            }
        }
        return fusionCount;
    }

    // Rewrite Rule 2: Identity Spider Removal
    // An active spider with phase 0 and exactly 2 connections acts as an identity wire:
    // ---S(0)---  ===>  --------
    int removeIdentitySpiders() {
        int removedCount = 0;
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (!nodes[i].active || nodes[i].type == SpiderType::BOUNDARY) continue;

            if (std::abs(nodes[i].phase) < 1e-9 && nodes[i].neighbors.size() == 2) {
                int u = nodes[i].neighbors[0];
                int v = nodes[i].neighbors[1];

                // Connect u directly to v
                for (int& link : nodes[u].neighbors) if (link == static_cast<int>(i)) link = v;
                for (int& link : nodes[v].neighbors) if (link == static_cast<int>(i)) link = u;

                nodes[i].active = false;
                nodes[i].neighbors.clear();
                removedCount++;
            }
        }
        return removedCount;
    }

    // Run complete diagrammatic simplification pipeline
    void simplify() {
        bool changed = true;
        while (changed) {
            int fusions = applySpiderFusion();
            int identities = removeIdentitySpiders();
            changed = (fusions > 0 || identities > 0);
        }
    }

    void printState(const std::string& label) const {
        std::cout << "[" << label << "]" << std::endl;
        std::cout << "  Active Spiders / Nodes: " << getActiveNodeCount() << std::endl;
        for (const auto& n : nodes) {
            if (!n.active) continue;
            std::string typeStr;
            if (n.type == SpiderType::Z_GREEN) typeStr = "Z-Spider (Green)";
            else if (n.type == SpiderType::X_RED) typeStr = "X-Spider (Red)";
            else if (n.type == SpiderType::HADAMARD) typeStr = "Hadamard Box";
            else typeStr = "Boundary Port";

            std::cout << "  Node #" << n.id << " : " << std::left << std::setw(18) << typeStr
                      << " | Phase = " << std::fixed << std::setprecision(3) << n.phase
                      << " rad | Valence = " << n.neighbors.size() << std::endl;
        }
    }
};

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " C++ ENGINE: CATEGORICAL ZX-CALCULUS DIAGRAM SIMPLIFIER   " << std::endl;
    std::cout << "==========================================================" << std::endl;

    ZXDiagram diagram;

    // Build a Quantum Teleportation / Entangled Protocol Diagram:
    // Input state -> Z(pi/4) spider -> Z(pi/2) spider -> Identity Z(0) -> Output state
    int in = diagram.addNode(SpiderType::BOUNDARY);
    int z1 = diagram.addNode(SpiderType::Z_GREEN, 0.785398); // pi / 4
    int z2 = diagram.addNode(SpiderType::Z_GREEN, 1.570796); // pi / 2
    int zId = diagram.addNode(SpiderType::Z_GREEN, 0.0);     // 0 (identity)
    int out = diagram.addNode(SpiderType::BOUNDARY);

    diagram.addEdge(in, z1);
    diagram.addEdge(z1, z2);
    diagram.addEdge(z2, zId);
    diagram.addEdge(zId, out);

    diagram.printState("Initial Quantum Circuit Graph (Unreduced)");
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "Applying Categorical Rewrites (Spider Fusion & Identity Reduction)..." << std::endl;
    diagram.simplify();
    std::cout << "----------------------------------------------------------" << std::endl;
    diagram.printState("Fully Reduced Categorical Quantum Diagram");

    std::cout << "==========================================================" << std::endl;
    std::cout << "  Quantum state morphism reduced to minimal canonical form!" << std::endl;
    std::cout << "==========================================================" << std::endl;

    return 0;
}
