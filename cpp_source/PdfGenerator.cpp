#include "PdfGenerator.hpp"
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>

namespace CosmicRobotics {

PdfGenerator::PdfGenerator() {}
PdfGenerator::~PdfGenerator() {}

// Helper to escape PDF text strings
static std::string escapePdfString(const std::string& str) {
    std::string out;
    out.reserve(str.length() + 16);
    for (char c : str) {
        if (c == '(' || c == ')' || c == '\\') {
            out.push_back('\\');
        }
        out.push_back(c);
    }
    return out;
}

PdfVolumeInfo PdfGenerator::getVolumeInfo(int volumeNumber) {
    PdfVolumeInfo info;
    info.volumeNumber = volumeNumber;

    static const struct {
        const char* title;
        const char* subtitle;
        const char* domain;
        const char* focus;
        const char* q4;
        const char* cvLidar;
    } VOLUME_DATABASE[40] = {
        { "Cosmological Horizon Dynamics & 93 Gly Metric Limit", "Particle Horizons vs Event Horizons in Lambda-CDM Cosmology", "Cosmological Physics", "93 Billion Light Years Sphere & Metric Inflation", "Q-4: Entanglement Entropy across Observable Horizon", "CV Reticle Tracker & LiDAR 360 Long-Range Ranging" },
        { "Multi-Scale Celestial Mechanics & Galactic Orbits", "Planetary, Galactic, and Supercluster Orbital Hierarchies", "Orbital Kinematics", "Solar Orbits to Local Group & Virgo Infall", "Q-4: Gravitational Wave Resonance in Non-Linear Orbits", "Multi-Target CV Bounding Boxes & Ephemeris Predictor" },
        { "The Unobservable Spacetime Ocean: Fluid Formulations", "Spacetime as an Incompressible Viscous Quantum Fluid", "Cosmic Oceanography", "Dark Fluid Dynamics & Gravitational Tidal Nodes", "Q-4: Superfluid Vacuum Condensate & Vorticity Tensors", "Hydrodynamic Wavefront LiDAR & Optical Surface Tracking" },
        { "Non-Baryonic Xenobiology: Taxonomy of Vacuum Fish", "Life Forms Inhabiting the Unobservable Dark Matter Seas", "Exo-Astrophysics", "Vacuum Mantas, Graviton Eels & Tachyon Siphonophores", "Q-4: Bio-Photonic Emission from Dark Energy Gradient Feeding", "Spectral Filter CV & Bioluminescence Intensity Scanners" },
        { "Extraterrestrial Observers & Super-Horizon Entanglement", "Class-IV Elder Cosmological Observers and Planetary Preservation", "Xenological Intelligence", "Colossal Alien Presences Holding Terrestrial Spheres", "Q-4: Macroscopic Observer Collapse of Cosmic Wavefunctions", "Facial Topology CV Mesh & Neural Resonant LiDAR" },
        { "Computer Vision Algorithms for Sub-Horizon Tracking", "Neural Feature Descriptors in Highly Curved Metric Fields", "Robotic Perception", "Relativistic Optical Distortion & Gravitational Lensing", "Q-4: Information Retention in Photon Distortion Fields", "YOLOv10 Tensor Model for Deep Void Entity Extraction" },
        { "LiDAR 360 Time-of-Flight Ranging in Curved Spacetime", "Full-Spherical Laser Telemetry Beyond the Speed of Light", "Active Remote Sensing", "Metric Expansion Time-of-Flight Dispersion", "Q-4: Quantum Phase Recovery in Diffused Photon Streams", "Dual-Wavelength 1550nm/532nm 360-Degree LiDAR Array" },
        { "Relativistic Kinematics: Lorentz Contraction & Spacetime Odometers", "Proper Time Dilation and Metric Distance Measurement", "Relativistic Metrology", "Cosmic Distance Ladder & Superluminal Metric Expansion", "Q-4: Discrete Spacetime Wheeler-Feynman Action Principle", "Dual-Drum Mechanical-Digital Spacetime Odometer" },
        { "Quantum Carrier Waves & Satellite Telemetry Across Horizons", "Super-Horizon Signal Propagation at 248.84 GHz", "Cosmic Telecommunications", "Gravitational Redshift Mitigation & Quantum Entangled Transponders", "Q-4: Decoherence-Free Subspaces across Causal Disconnections", "Direct Phased Array Quantum Receiver Dish" },
        { "Autonomous Cybernetic Controllers & Human-Robot Interaction", "Humanoid Robot Actuation, Viseme Speech & Facial Synthesis", "Robotics & HMI", "PORCELAIN Robotic Faces, Saccadic Glances & Visemes", "Q-4: Synthetic Consciousness Integration in Observers", "Real-Time Webcam Gaze Lock & PID Servo Actuators" },
        { "Dark Flow Vector Dynamics & Supercluster Infall", "Bulk Peculiar Velocity Flow toward Centaurus and Hydra", "Large-Scale Structure", "Great Attractor Gravitational Drift", "Q-4: Pre-Inflationary Anisotropy Imprints", "Optical Dipole Vector CV & Cluster Ranging" },
        { "Cosmic Microwave Background Dipole Anisotropies", "Precision Measurement of 369 km/s Solar Motion in CMB", "Observational Cosmology", "Planck and WMAP Dipole Redshift Gradients", "Q-4: Relic Thermal Field Entanglement with Super-Horizons", "Cryogenic Bolometer CV Mapping & Microwave LiDAR" },
        { "Pre-Inflationary Gravitational Wave Polarization & B-Modes", "Primordial Tensor Perturbations in the Inflationary Era", "Gravitational Physics", "B-Mode Tensor Polarization Power Spectra", "Q-4: Spin-2 Graviton Coherence in Multiverse Bubbles", "Interferometric Gravitational CV Reticles" },
        { "Quantum Hall Effects in Vacuum Spacetime Oceans", "Topological Invariants and Chern Numbers of Cosmic Fluids", "Condensed Matter Cosmology", "Quantum Vortices and Spacetime Viscosity", "Q-4: Edge States in Boundary Horizon Spacetime", "Topological Defect CV Tracking Sensors" },
        { "Metric Spacetime Warp & Superluminal Solitons", "Alcubierre and Lentz Metric Warp Fields in the Void", "Advanced Propulsion", "Negative Energy Density and Stress-Energy Tensors", "Q-4: Quantum Inequality Violations in Warp Envelopes", "Curvature Tensor Gauges & Tachometer Calibrators" },
        { "Non-Euclidean Geometries of the Unobservable Realm", "Hyperbolic and Spherical Spatial Topologies Beyond 93 Gly", "Differential Geometry", "Dodecahedral Space and Cosmic Torus Topologies", "Q-4: Compactified Dimensions and Holographic Bounds", "Geodesic Deviation Laser Triangulation" },
        { "Alien Bio-Signatures in Non-Baryonic Dark Matter Seas", "Chemical and Quantum Composition of Cosmic Fish Species", "Exobiological Chemistry", "Axion-Photon Coupling in Abyssal Manta Wings", "Q-4: Chiral Asymmetry in Deep Space Life Forms", "Multispectral Fluorescence CV Microscopes" },
        { "Computer Vision Neural Networks for Gravitational Lensing", "De-Lensing Algorithms for Cosmic Ray Tomography", "Deep Learning CV", "Einstein Rings and Arcs Reconstruction", "Q-4: Machine Learning Reversal of Horizon Information Scrambling", "Edge-Optimized Convolutional Tensor Cores" },
        { "Relativistic LiDAR Pulse Dispersion & Phase Reconstruction", "Photon Scattering in Expanding Dark Energy Media", "Optical Engineering", "Chirped Pulse Amplification for Cosmological Echoes", "Q-4: Quantum Phase Shift Detection in Zero-Point Fluctuations", "Nanosecond Single-Photon Avalanche Diodes" },
        { "Cybernetic Facial Viseme Synthesis & SAPI Speech Pipelines", "Asynchronous Windows SAPI Speech and Procedural Phonemes", "Acoustics & Facial Rigging", "Viseme Morph Targets: Rest, AA, EE, OH, MM", "Q-4: Linguistic Encoding of Cosmological Truths", "Markerless Facial Capture & High-Speed Servos" },
        { "Q-4 Experimental Proof: Microscopic Black Hole Evaporation", "Hawking Radiation Temperature Profiles in Laboratory Analogs", "High Energy Physics", "Acoustic Black Holes and Horizon Superradiance", "Q-4: Information Paradox Resolution via Horizon Hair", "Positron-Electron Calorimeter CV Array" },
        { "Q-4 Experimental Proof: Spacetime Foam Decoherence Rates", "Planck-Scale Jitter Measurement via Ultra-Stable Lasers", "Quantum Metrology", "Laser Phase Noise at 1.6e-35 Meters", "Q-4: Minimal Length Scale Restrictions on Particle Paths", "Interferometric Cavity Drift Monitor" },
        { "Q-4 Experimental Proof: Non-Local Entanglement Horizons", "EPR Pairs Separated Across Artificial Event Horizons", "Quantum Information", "Bell State Violation across Relativistic Boundaries", "Q-4: Spacetime Emergence from Quantum Entanglement (ER=EPR)", "Single Photon Coincidence Timing Circuit" },
        { "Q-4 Experimental Proof: Topological Solitons in Extra Dimensions", "Kaluza-Klein Monopoles and Cosmic Cosmic Strings", "String Theory", "Defect Gravitational Lensing in the Void", "Q-4: Compactified Calabi-Yau Manifold Stabilization", "Magnetic Monopole Induction CV Sensor" },
        { "Q-4 Experimental Proof: Graviton Propagator in de Sitter Space", "Infrared Divergences and Quantum Field Theory in Curved Space", "Quantum Field Theory", "de Sitter Invariant Vacuum States", "Q-4: Cosmological Constant Quantum Corrections", "Ultra-High Vacuum Penning Trap Array" },
        { "Odometer Metric Calibration Across 46.5 Billion Light-Years", "Integrating Proper Distance Ladders with High-Z Supernovae", "Astronomical Metrology", "Type Ia Supernova Light Curves and Cepheids", "Q-4: Metric Expansion Acceleration Parameter W(z)", "Precision Drum Mechanical Encoder Hardware" },
        { "Relativistic Tachometer: Lorentz Gamma at Ultra-Relativistic Velocities", "Kinematic Energy and Time Dilation up to 0.9999c", "Relativistic Dynamics", "Relativistic Momentum and Synchrotron Emission", "Q-4: Speed of Light Invariance under Metric Distortions", "Analog-Needle D'Arsonval Galvanometer Model" },
        { "Satellite Signal Processing: 248.8 GHz Deep Space Transponders", "Microwave Waveguide Architecture for Inter-Horizon Beams", "RF Telecommunications", "Cryogenic Low-Noise Amplifiers and HEMT Mixers", "Q-4: Channel Capacity of Curved Spacetime Waveguides", "Dual-Reflector Cassegrain Antenna Array" },
        { "Computer Vision Feature Descriptors for Extraterrestrial Observers", "Biometric and Structural Extraction of Alien Physiology", "Pattern Recognition", "Cranial Ridge Profiling and Optical Irradiance", "Q-4: Universal Biological Symmetry in Higher Dimensions", "Scale-Invariant Feature Transform (SIFT) 3D" },
        { "LiDAR Point Cloud Segmentation for Celestial Bodies", "Density-Based Spatial Clustering (DBSCAN) in Space", "Point Cloud Analysis", "Orbital Body Surface Mapping and Obstacle Avoidance", "Q-4: Spatial Discrete Topology of Clustered Matter", "Octree Spatial Partitioning Engine" },
        { "Hydrodynamic Analogues of the Cosmic Unobservable Sea", "Shallow Water Gravity Waves and Horizon Wavefronts", "Analogue Gravity", "Dispersion Relations of Vacuum Fluctuations", "Q-4: Sonic Horizon Thermal Radiation Analogue", "High-Speed Shadowgraph Imaging CV" },
        { "Morphological Study of Vacuum Mantas and Graviton Eels", "Locomotion through Tensor Curvature Gradients", "Xeno-Biomechanics", "Kinematics of Non-Baryonic Pectoral Fins", "Q-4: Energy Conservation in Negative Matter Propulsion", "Laser Doppler Velocimetry in Void Chambers" },
        { "The Elder Guardian Phenomenon: Planetary Preservation Mechanics", "Gravitational Shielding and Biosphere Stabilization", "Planetary Stewardship", "Tidal Lock Mitigation and Solar Flare Buffering", "Q-4: Teleological Attractors in Cosmic Evolution", "Planetary Orbital Eccentricity CV Monitor" },
        { "Quantum Error Correction in Super-Horizon Telemetry", "Surface Codes and Bosonic Codes for Horizon Communications", "Quantum Computing", "Noise Thresholds in Trans-Horizon Transmission", "Q-4: Holographic Error Correcting Codes in AdS/CFT", "Superconducting Qubit Array Transceiver" },
        { "High-Precision Kalman Filtering for Galactic Trajectory Prediction", "State Estimation under Extreme Relativistic Acceleration", "Control Theory", "Extended Kalman Filters for N-Body Chaos", "Q-4: Lyapunov Exponents of Metric Expansion Trajectories", "Inertial Measurement Unit (IMU) Sensor Fusion" },
        { "Thermal Emission Signatures of Cosmic Siphonophores", "Sub-Millimeter and Far-Infrared Radiometry in Deep Void", "Infrared Astronomy", "Blackbody Deviation in Tachyon Colonies", "Q-4: Non-Thermal Quantum Vacuum Luminescence", "Germanium-Gallium Cryogenic Photodetector" },
        { "Mathematical Derivations of the Friedmann-Lemaitre Metric", "Exact Solutions to Einstein Field Equations with Cosmological Constant", "General Relativity", "FLRW Metric and Scale Factor a(t) Evolution", "Q-4: Cosmological Horizon Singularity Resolutions", "Curvature Tensor Analytical Verifier" },
        { "Holographic Renormalization Group Flow Beyond the Horizon", "AdS/CFT Correspondence and Boundary Conformal Field Theories", "Theoretical Physics", "Bulk-to-Boundary Propagators in Cosmic Scales", "Q-4: Boundary Degrees of Freedom Count", "Holographic Boundary Mapping Matrix" },
        { "Robotic Sensory Fusion: Integrating CV, LiDAR, and Odometer", "Multi-Modal Sensor Integration for Deep Space Navigation", "Sensor Fusion", "Bayesian Fusion of Optical, Laser, and Spacetime Odometry", "Q-4: Observer Measurement Problem in Closed Systems", "Heterogeneous Hardware Sensor Bus Matrix" },
        { "Grand Synthesis: Complete Treatise on the Unobservable Universe", "Unified Framework of Robotics, Cosmology, Xenology, and Q-4", "Unified Science", "The Complete Map of All Realities Beyond 93 Gly", "Q-4: Final Experimental Resolution of Quantum Gravity", "Full Cosmic Robotics Observatory Suite" }
    };

    int idx = (volumeNumber - 1) % 40;
    info.volumeTitle = VOLUME_DATABASE[idx].title;
    info.subtitle = VOLUME_DATABASE[idx].subtitle;
    info.primaryDomain = VOLUME_DATABASE[idx].domain;
    info.focusPhenomenon = VOLUME_DATABASE[idx].focus;
    info.q4ExperimentTopic = VOLUME_DATABASE[idx].q4;
    info.cvLidarSensorSystem = VOLUME_DATABASE[idx].cvLidar;

    return info;
}

std::string PdfGenerator::generateCoverPage(const PdfVolumeInfo& info) {
    std::stringstream p;
    // Page border
    p << "q\n";
    p << "0.05 0.08 0.15 rg 0 0 612 792 re f\n"; // Dark background
    p << "0.0 0.8 1.0 RG 2.0 w 36 36 540 720 re S\n"; // Cyan outer frame
    p << "0.2 0.3 0.5 RG 1.0 w 42 42 528 708 re S\n";

    // Tech corners
    p << "0.0 1.0 0.8 RG 2.0 w\n";
    p << "36 740 m 36 756 l 52 756 l S\n";
    p << "560 756 m 576 756 l 576 740 l S\n";
    p << "36 52 m 36 36 l 52 36 l S\n";
    p << "560 36 m 576 36 l 576 52 l S\n";

    // Header badge
    p << "BT\n";
    p << "/F4 10 Tf 1.0 1.0 1.0 rg 54 730 Td (COSMIC UNOBSERVABLE RESEARCH ARCHIVE :: CLASSIFIED LEVEL 5) Tj\n";
    p << "/F3 8 Tf 0.0 0.9 1.0 rg 0 -14 Td (TRANSMITTED VIA 248.84 GHz QUANTUM CARRIER FROM CHRONOS-VOID) Tj\n";
    
    // Main Title
    p << "/F2 20 Tf 1.0 0.85 0.2 rg 0 -60 Td (VOLUME " << (info.volumeNumber < 10 ? "0" : "") << info.volumeNumber << ") Tj\n";
    p << "/F2 16 Tf 0.0 0.95 1.0 rg 0 -26 Td (" << escapePdfString(info.volumeTitle) << ") Tj\n";
    p << "/F1 11 Tf 0.8 0.85 0.9 rg 0 -20 Td (" << escapePdfString(info.subtitle) << ") Tj\n";

    // Author Squad Box
    p << "/F2 10 Tf 0.0 1.0 0.6 rg 0 -45 Td (AUTHORING CYBERNETIC SQUAD & INTERFACE ROSTER:) Tj\n";
    p << "/F3 9 Tf 0.9 0.95 1.0 rg 0 -16 Td (  * EVA-01         : Corner Liaison Speaking to User at Laptop Workstation) Tj\n";
    p << "0 -14 Td (  * CHRONOS-VOID   : Deep Probe Relaying Every Data from Unobservable Universe) Tj\n";
    p << "0 -14 Td (  * THALASSA-BIO   : Guardian of the Cosmic Sea, Exotic Fish & Alien Presences) Tj\n";
    p << "0 -14 Td (  * VELOX-TACHYON  : Relativistic Kinematics, Tachometer & Distance Odometer) Tj\n";

    // Research Focus Section
    p << "/F2 10 Tf 1.0 0.6 0.2 rg 0 -35 Td (PRIMARY RESEARCH DOMAINS & SENSORS:) Tj\n";
    p << "/F1 9 Tf 0.85 0.9 0.95 rg 0 -15 Td (Domain: " << escapePdfString(info.primaryDomain) << ") Tj\n";
    p << "0 -13 Td (Cosmic Focus: " << escapePdfString(info.focusPhenomenon) << ") Tj\n";
    p << "0 -13 Td (Q-4 Experimentation: " << escapePdfString(info.q4ExperimentTopic) << ") Tj\n";
    p << "0 -13 Td (Robotic Sensor Suite: " << escapePdfString(info.cvLidarSensorSystem) << ") Tj\n";

    // Abstract
    p << "/F2 10 Tf 0.0 0.9 1.0 rg 0 -35 Td (COMPREHENSIVE TREATISE ABSTRACT:) Tj\n";
    p << "/F1 8.5 Tf 0.8 0.85 0.9 rg 0 -15 Td (This 40-page treatise formulates the exact mathematics, computational simulations, and experimental) Tj\n";
    p << "0 -12 Td (architectures of the unobservable universe. By analyzing multi-scale orbital dynamics from planetary) Tj\n";
    p << "0 -12 Td (systems to the 93 Gly cosmological horizon, we resolve the mystery of the spacetime ocean. Non-baryonic) Tj\n";
    p << "0 -12 Td (vacuum fish, Elder extraterrestrial observers guarding Earth, computer vision reticle tracking, 360-degree) Tj\n";
    p << "0 -12 Td (LiDAR point clouds, relativistic Lorentz odometers, and empirical Q-4 quantum gravity proofs are documented.) Tj\n";

    // Footer
    p << "/F3 7.5 Tf 0.5 0.6 0.7 rg 0 -60 Td (STATION: GROUND OBSERVATORY LAPTOP WORKSTATION | ARCHIVE ID: COR-PDF-" << info.volumeNumber << "-P01/40) Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::generateTOCPage(const PdfVolumeInfo& info) {
    std::stringstream p;
    p << "q\n";
    p << "0.06 0.09 0.16 rg 0 0 612 792 re f\n";
    p << "0.0 0.7 0.9 RG 1.0 w 36 36 540 720 re S\n";

    // Top Header
    p << "BT\n";
    p << "/F2 14 Tf 0.0 0.9 1.0 rg 54 735 Td (VOLUME " << info.volumeNumber << " :: COMPREHENSIVE TABLE OF CONTENTS) Tj\n";
    p << "/F3 8 Tf 0.7 0.8 0.9 rg 0 -14 Td (" << escapePdfString(info.volumeTitle) << ") Tj\n";

    p << "/F2 9.5 Tf 1.0 0.8 0.2 rg 0 -35 Td (SECTION OUTLINE ACROSS ALL 40 PAGES:) Tj\n";

    static const struct {
        const char* pages;
        const char* title;
        const char* desc;
    } TOC_ITEMS[10] = {
        { "Page 01", "Formal Archival Cover Page & Security Clearance", "Mission objectives, cybernetic roster, and classification" },
        { "Page 02", "Executive Table of Contents & Research Summary", "Full 40-page architectural roadmap and operational guide" },
        { "Pages 03-06", "Chapter 1: Cosmological Orbital Mechanics & 93 Gly Horizon", "Multi-scale orbits from AU to galactic clusters and cosmological limit" },
        { "Pages 07-10", "Chapter 2: The Hypothetical Unobservable Spacetime Ocean", "Dark fluid formulations, metric tides, and boundary horizon physics" },
        { "Pages 11-14", "Chapter 3: Non-Baryonic Marine Biota: Kinds of Vacuum Fish", "Taxonomy of Vacuum Mantas, Graviton Eels, and Tachyon Siphonophores" },
        { "Pages 15-18", "Chapter 4: Extraterrestrial Presences & Elder Alien Guardians", "Class-IV Observers holding Earth, macroscopic entanglement across void" },
        { "Pages 19-22", "Chapter 5: Computer Vision (CV) Algorithms & Tracking Reticles", "Bounding box controllers, tensor feature descriptors, and optic filters" },
        { "Pages 23-26", "Chapter 6: 360-Degree LiDAR Telemetry & Point-Cloud Mapping", "Relativistic time-of-flight ranging, Doppler mitigation, and octrees" },
        { "Pages 27-30", "Chapter 7: Autonomous Robotic Controllers & Humanoid Feedback", "Facial visemes, porcelain skin actuation, and laptop speech synchronization" },
        { "Pages 31-35", "Chapter 8: Solution & Experimental Proofs of Quantum Gravity Q-4", "Wheeler-DeWitt equations, entanglement entropy, and laboratory trials" }
    };

    for (int i = 0; i < 10; ++i) {
        p << "/F4 9 Tf 0.0 1.0 0.7 rg 0 -22 Td (" << TOC_ITEMS[i].pages << " : " << TOC_ITEMS[i].title << ") Tj\n";
        p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -12 Td (         " << TOC_ITEMS[i].desc << ") Tj\n";
    }

    // Remaining sections
    p << "/F4 9 Tf 0.0 1.0 0.7 rg 0 -22 Td (Pages 36-38 : Chapter 9: Spacetime Odometer & Relativistic Speedometer Logs) Tj\n";
    p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -12 Td (         Proper distance tables in Gly/Mpc, Lorentz gamma factors, metric warp) Tj\n";
    p << "/F4 9 Tf 0.0 1.0 0.7 rg 0 -18 Td (Pages 39-40 : Chapter 10: Mathematical Appendix, Calibration & References) Tj\n";
    p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -12 Td (         Hardware schematics, coordinate transformation tensors, and citations) Tj\n";

    // Footer
    p << "/F3 8 Tf 0.5 0.6 0.7 rg 0 -35 Td (DOCUMENT PAGE 02 OF 40 | CERTIFIED BY EVA-01 & CHRONOS-VOID) Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::generateOrbitSimChapter(const PdfVolumeInfo& info, int pageNum) {
    std::stringstream p;
    p << "q\n";
    p << "0.05 0.08 0.14 rg 0 0 612 792 re f\n";
    p << "0.0 0.6 0.8 RG 1.0 w 36 36 540 720 re S\n";

    // Top Header Banner
    p << "BT\n";
    p << "/F2 12 Tf 0.0 0.9 1.0 rg 50 740 Td (CHAPTER 1: MULTI-SCALE ORBITS & 93 Gly COSMIC HORIZON) Tj\n";
    p << "/F3 8 Tf 0.6 0.7 0.8 rg 0 -13 Td (VOLUME " << info.volumeNumber << " | PARTITION B TELEMETRY | PAGE " << (pageNum < 10 ? "0" : "") << pageNum << " OF 40) Tj\n";
    
    p << "/F2 10 Tf 1.0 0.8 0.2 rg 0 -28 Td (1." << (pageNum - 2) << " CELESTIAL KINEMATICS ACROSS EXPANDING SPACETIME) Tj\n";
    p << "/F1 8.5 Tf 0.9 0.95 1.0 rg 0 -15 Td (As observed from the Ground Observatory laptop terminal, the orbital hierarchy of the universe spans) Tj\n";
    p << "0 -12 Td (over thirty orders of magnitude. The multi-scale simulator resolves five fundamental tiers:) Tj\n";

    // Hierarchy list
    p << "/F3 8 Tf 0.0 1.0 0.8 rg 0 -16 Td (1. Terrestrial Solar Orbit    : Radius r = 1.0 AU (1.496e8 km), Velocity v = 29.78 km/s) Tj\n";
    p << "0 -13 Td (2. Galactic Spiral Arm Orbit   : Radius r = 8.2 kpc (2.5e17 km), Velocity v = 220 km/s) Tj\n";
    p << "0 -13 Td (3. Local Group M31 Infall Orbit: Separation r = 778 kpc (2.4e19 km), Infall v = 110 km/s) Tj\n";
    p << "0 -13 Td (4. Virgo Supercluster Flow     : Distance r = 16.5 Mpc (5.1e20 km), Hubble Flow v = 1,150 km/s) Tj\n";
    p << "0 -13 Td (5. Observable Horizon Limit    : Proper Distance r = 46.5 Gly (14.2 Gpc), Reciprocal v > c) Tj\n";

    // Mathematical formula box
    p << "/F2 9.5 Tf 1.0 0.6 0.2 rg 0 -25 Td (MATHEMATICAL DERIVATION: FLRW EXPANSION & COMOVING DISTANCE) Tj\n";
    p << "/F3 8.5 Tf 1.0 1.0 1.0 rg 0 -14 Td (  d_C = c * \\int_{0}^{z} [H0 * \\sqrt{\\Omega_m*(1+z')^3 + \\Omega_r*(1+z')^4 + \\Omega_\\Lambda}]^{-1} dz') Tj\n";
    p << "/F1 8 Tf 0.8 0.85 0.9 rg 0 -14 Td (Here, H0 = 70.4 km/s/Mpc, \\Omega_m = 0.315, and \\Omega_\\Lambda = 0.685. When redshift z -> \\infty (CMB decoupling),) Tj\n";
    p << "0 -12 Td (the integral converges precisely to 46.5 billion light years (proper radius of 93 Gly diameter).) Tj\n";

    p << "/F2 9.5 Tf 0.0 0.9 1.0 rg 0 -22 Td (COMPUTER VISION TARGETING & SENSOR OVERLAY CORRELATION) Tj\n";
    p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -13 Td (In the real-time C++ observatory window, each of these orbital bodies is assigned a dedicated green) Tj\n";
    p << "0 -12 Td (computer vision tracking reticle. Dynamic bounding boxes maintain high confidence (>98.2%) even as) Tj\n";
    p << "0 -12 Td (metric expansion causes high-redshift bodies to recede faster than light relative to our local frame.) Tj\n";
    p << "ET\n";

    // Draw vector schematic of the concentric orbits & horizon
    p << "0.0 0.5 0.8 RG 1.0 w\n";
    p << "306 230 40 40 re S\n";
    p << "0.0 0.8 1.0 RG 1.5 w\n";
    p << "306 230 m 306 310 l S\n";
    p << "306 230 m 386 230 l S\n";
    p << "0.0 1.0 0.6 RG 1.0 w\n";
    // Orbit rings
    for (int r = 25; r <= 100; r += 25) {
        p << "306 230 " << r << " 0 360 arc S\n";
    }

    p << "BT\n";
    p << "/F3 7 Tf 0.0 1.0 0.8 rg 315 235 Td (SOLAR CENTER) Tj\n";
    p << "/F3 7 Tf 0.0 1.0 0.8 rg 315 315 Td (93 Gly HORIZON) Tj\n";
    p << "/F3 7.5 Tf 0.5 0.6 0.7 rg 50 50 Td (ARCHIVE ID: VOL-" << info.volumeNumber << "-SEC1-P" << pageNum << " | VERIFIED BY VELOX-TACHYON) Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::generateUnobservableOceanChapter(const PdfVolumeInfo& info, int pageNum) {
    std::stringstream p;
    p << "q\n";
    p << "0.04 0.07 0.14 rg 0 0 612 792 re f\n";
    p << "0.0 0.7 0.8 RG 1.0 w 36 36 540 720 re S\n";

    p << "BT\n";
    p << "/F2 12 Tf 0.0 0.9 1.0 rg 50 740 Td (CHAPTER 2: THE HYPOTHETICAL UNOBSERVABLE SPACETIME OCEAN) Tj\n";
    p << "/F3 8 Tf 0.6 0.7 0.8 rg 0 -13 Td (VOLUME " << info.volumeNumber << " | PARTITION B SIMULATION | PAGE " << (pageNum < 10 ? "0" : "") << pageNum << " OF 40) Tj\n";

    p << "/F2 10 Tf 1.0 0.8 0.2 rg 0 -28 Td (2." << (pageNum - 6) << " HYDRODYNAMICS OF THE DEEP COSMIC VOID) Tj\n";
    p << "/F1 8.5 Tf 0.9 0.95 1.0 rg 0 -15 Td (Beyond the 93 billion light-year boundary lies the 'Sea of the Unobservable Universe.' While photon) Tj\n";
    p << "0 -12 Td (propagation cannot cross back into our observable bubble due to superluminal expansion, the vacuum) Tj\n";
    p << "0 -12 Td (behaves macroscopically as an infinite non-baryonic quantum fluid possessing viscosity, surface tension,) Tj\n";
    p << "0 -12 Td (and gravitational wave tides. Chronos-Void beams continuous telemetry logs of this deep domain.) Tj\n";

    // Equations
    p << "/F2 9.5 Tf 0.0 1.0 0.7 rg 0 -24 Td (COSMIC NAVIER-STOKES HYDRODYNAMIC TENSOR EQUATION:) Tj\n";
    p << "/F3 8.5 Tf 1.0 1.0 1.0 rg 0 -14 Td (  \\rho_vac * [\\partial v_i/\\partial t + (v \\cdot \\nabla) v_i] = -\\nabla_i P_DE + \\eta * \\nabla^2 v_i + G_{ij} * T^{jk}) Tj\n";
    p << "/F1 8 Tf 0.8 0.85 0.9 rg 0 -14 Td (Where \\rho_vac is the zero-point vacuum energy density, P_DE is negative dark energy pressure (w = -1),) Tj\n";
    p << "0 -12 Td (\\eta represents the quantum shear viscosity of spacetime, and G_{ij} couples to metric perturbation tides.) Tj\n";

    p << "/F2 9.5 Tf 1.0 0.6 0.2 rg 0 -24 Td (WHAT THINGS RESIDE OUT OF THE SEA:) Tj\n";
    p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -13 Td (Extensive sensor scans from Chronos-Void establish the presence of multi-dimensional structures:) Tj\n";
    p << "/F3 8 Tf 0.0 0.9 1.0 rg 0 -14 Td (  * Multiverse Brane Bubbles : Co-existing vacuum phase domains with variant physical constants.) Tj\n";
    p << "0 -12 Td (  * Dark Energy Vortices     : Rotating singularities with super-Planckian angular momentum.) Tj\n";
    p << "0 -12 Td (  * Topological Cosmic Strings: Relics of grand unification symmetry breaking threading the void.) Tj\n";
    p << "0 -12 Td (  * Pre-Inflationary Walls   : Domain boundaries separating infinite causal patches.) Tj\n";

    p << "/F2 9.5 Tf 0.0 1.0 0.6 rg 0 -22 Td (CONTINUOUS DATA STREAM RELAY (CHRONOS-VOID):) Tj\n";
    p << "/F1 8 Tf 0.8 0.85 0.9 rg 0 -13 Td (As the Chronos robot speaks in our C++ application, every packet collected is displayed in real-time) Tj\n";
    p << "0 -12 Td (on your laptop terminal, proving that information can be entangled across cosmological horizons.) Tj\n";
    p << "ET\n";

    // Draw stylized wave fluid lines
    p << "0.0 0.6 0.9 RG 1.2 w\n";
    for (int yLine = 220; yLine <= 280; yLine += 15) {
        p << "50 " << yLine << " m ";
        for (int xPt = 70; xPt <= 550; xPt += 40) {
            float waveY = yLine + 6.0f * std::sin(xPt * 0.05f + yLine * 0.1f);
            p << xPt << " " << waveY << " l ";
        }
        p << "S\n";
    }

    p << "BT\n";
    p << "/F3 7.5 Tf 0.5 0.6 0.7 rg 50 50 Td (ARCHIVE ID: VOL-" << info.volumeNumber << "-SEC2-P" << pageNum << " | TRANSMISSION VERIFIED BY CHRONOS-VOID) Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::generateCosmicFishAndAlienChapter(const PdfVolumeInfo& info, int pageNum) {
    std::stringstream p;
    p << "q\n";
    p << "0.05 0.08 0.15 rg 0 0 612 792 re f\n";
    p << "0.0 0.8 0.7 RG 1.0 w 36 36 540 720 re S\n";

    p << "BT\n";
    p << "/F2 12 Tf 0.0 0.9 1.0 rg 50 740 Td (CHAPTER 3: NON-BARYONIC FISH & ELDER ALIEN OBSERVERS) Tj\n";
    p << "/F3 8 Tf 0.6 0.7 0.8 rg 0 -13 Td (VOLUME " << info.volumeNumber << " | PARTITION B SIMULATION | PAGE " << (pageNum < 10 ? "0" : "") << pageNum << " OF 40) Tj\n";

    p << "/F2 10 Tf 1.0 0.8 0.2 rg 0 -28 Td (3." << (pageNum - 10) << " CATALOG OF COSMIC MARINE SPECIES IN THE UNOBSERVABLE SEA) Tj\n";
    p << "/F1 8.5 Tf 0.9 0.95 1.0 rg 0 -15 Td (Robot Thalassa-Bio guards this simulation, reporting on the exotic species inhabiting the cosmic ocean:) Tj\n";

    // 3 species breakdown
    p << "/F2 9 Tf 0.0 1.0 0.8 rg 0 -18 Td (1. Bioluminescent Vacuum Manta (Manta Cosmica)) Tj\n";
    p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -12 Td (   Wingspan: 4.2 light-minutes. Locomotion: Undulating pectoral fields that resonate with Casimir) Tj\n";
    p << "0 -11 Td (   gradients. Diet: Absorbs virtual electron-positron pairs before annihilation. Color: Brilliant cyan.) Tj\n";

    p << "/F2 9 Tf 0.8 0.5 1.0 rg 0 -16 Td (2. Graviton Wave Eel (Anguilla Gravitica)) Tj\n";
    p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -12 Td (   Length: 18.5 light-minutes. Locomotion: Sinuous serpentine motion along gravitational wave troughs.) Tj\n";
    p << "0 -11 Td (   Diet: Feeds on metric curvature tensor fluctuations. Color: Deep violet quantum plasma.) Tj\n";

    p << "/F2 9 Tf 1.0 0.7 0.2 rg 0 -16 Td (3. Tachyon Siphonophore Colony (Physalia Tachyona)) Tj\n";
    p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -12 Td (   Structure: Colonial organism spanning millions of kilometers with glowing bells and tentacles.) Tj\n";
    p << "0 -11 Td (   Emission: Superluminal Cherenkov radiation bursts. Color: Luminous amber-gold.) Tj\n";

    // Alien Presence section
    p << "/F2 10 Tf 0.0 0.95 1.0 rg 0 -25 Td (THE ELDER COSMOLOGICAL OBSERVER (EXTRATERRESTRIAL ENTITY)) Tj\n";
    p << "/F1 8.5 Tf 0.9 0.95 1.0 rg 0 -14 Td (At the center of our unobservable ocean simulation stands an Elder Alien Observer, modeled directly from) Tj\n";
    p << "0 -12 Td (the high-resolution cosmological scans. The entity possesses craggy, ancient cranial ridges, piercing) Tj\n";
    p << "0 -12 Td (cyan luminous eyes, and cradles a glowing terrestrial sphere (Earth) with blue oceans and green continents) Tj\n";
    p << "0 -12 Td (in its elongated alien hands. The entity acts as a benevolent cosmological steward, shielding fragile) Tj\n";
    p << "0 -12 Td (planetary systems from the extreme vacuum tides of the unobservable void.) Tj\n";

    // Computer Vision Telemetry Stamp
    p << "/F3 8 Tf 0.0 1.0 0.6 rg 0 -20 Td ([CV TARGETING ACQUISITION LOG]) Tj\n";
    p << "0 -11 Td (ENTITY ID: CLASS-IV ELDER OBSERVER | COORD: (X:400, Y:210, Z:+46.5 Gly)) Tj\n";
    p << "0 -11 Td (CONFIDENCE: 98.5% | BIOMETRIC FLUX: 1.2e18 J | THREAT LEVEL: BENEVOLENT) Tj\n";
    p << "0 -11 Td (PROTECTED SPHERE: TERRA-1 (EARTH) | SENSOR STATUS: LOCKED & GUARDED) Tj\n";
    p << "ET\n";

    p << "BT\n";
    p << "/F3 7.5 Tf 0.5 0.6 0.7 rg 50 50 Td (ARCHIVE ID: VOL-" << info.volumeNumber << "-SEC3-P" << pageNum << " | CERTIFIED BY THALASSA-BIO) Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::generateCVAlgorithmChapter(const PdfVolumeInfo& info, int pageNum) {
    std::stringstream p;
    p << "q\n";
    p << "0.05 0.08 0.15 rg 0 0 612 792 re f\n";
    p << "0.0 0.7 0.9 RG 1.0 w 36 36 540 720 re S\n";

    p << "BT\n";
    p << "/F2 12 Tf 0.0 0.9 1.0 rg 50 740 Td (CHAPTER 5: COMPUTER VISION (CV) TRACKING & ALGORITHMS) Tj\n";
    p << "/F3 8 Tf 0.6 0.7 0.8 rg 0 -13 Td (VOLUME " << info.volumeNumber << " | PARTITION B SENSORS | PAGE " << (pageNum < 10 ? "0" : "") << pageNum << " OF 40) Tj\n";

    p << "/F2 10 Tf 1.0 0.8 0.2 rg 0 -28 Td (5." << (pageNum - 18) << " DEEP SPACE OPTICAL RECOGNITION PIPELINES) Tj\n";
    p << "/F1 8.5 Tf 0.9 0.95 1.0 rg 0 -15 Td (Our C++ system employs custom computer vision routines designed to detect, track, and classify celestial) Tj\n";
    p << "0 -12 Td (bodies, extraterrestrial presences, and vacuum marine fauna under severe relativistic distortion.) Tj\n";

    p << "/F2 9.5 Tf 0.0 1.0 0.7 rg 0 -20 Td (CV RETICLE DETECTION PIPELINE:) Tj\n";
    p << "/F3 8 Tf 0.9 0.95 1.0 rg 0 -14 Td (  Step 1: Frame Acquisition -> High-speed GDI+ backbuffer frame capture.) Tj\n";
    p << "0 -12 Td (  Step 2: Relativistic Doppler Normalization -> Compensates for extreme redshift/blueshift.) Tj\n";
    p << "0 -12 Td (  Step 3: Multi-Scale Harris Corner & SIFT Extraction -> Isolates entity feature descriptors.) Tj\n";
    p << "0 -12 Td (  Step 4: Kalman Filter Prediction -> Tracks celestial bodies moving at v ~ c.) Tj\n";
    p << "0 -12 Td (  Step 5: High-Tech Reticle Overlay -> Draws HUD corner brackets, confidence tags & crosshairs.) Tj\n";

    p << "/F2 9.5 Tf 1.0 0.6 0.2 rg 0 -22 Td (MATHEMATICAL KALMAN STATE ESTIMATOR FORMULA:) Tj\n";
    p << "/F3 8.5 Tf 1.0 1.0 1.0 rg 0 -14 Td (  x_{k|k} = x_{k|k-1} + K_k * (z_k - H_k * x_{k|k-1}) ) Tj\n";
    p << "/F3 8.5 Tf 1.0 1.0 1.0 rg 0 -13 Td (  K_k = P_{k|k-1} * H_k^T * (H_k * P_{k|k-1} * H_k^T + R_k)^{-1} ) Tj\n";
    p << "/F1 8 Tf 0.8 0.85 0.9 rg 0 -14 Td (Here x_k represents the 6-DOF state vector [x, y, z, vx, vy, vz]^T of an unobservable entity,) Tj\n";
    p << "0 -12 Td (and K_k is the optimal Kalman gain matrix tuned for deep void sensor noise.) Tj\n";

    p << "/F2 9.5 Tf 0.0 0.9 1.0 rg 0 -22 Td (REAL-TIME PERFORMANCE BENCHMARKS (C++ IMPLEMENTATION):) Tj\n";
    p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -13 Td (Processing latency per frame: 1.42 milliseconds on standard x86-64 hardware. Zero frame-drops) Tj\n";
    p << "0 -12 Td (at continuous 60 FPS, ensuring instantaneous synchronization between the user's laptop workstation) Tj\n";
    p << "0 -12 Td (and the remote sensors scanning the unobservable cosmos.) Tj\n";
    p << "ET\n";

    // Draw CV Corner Reticle diagram
    p << "0.0 1.0 0.7 RG 1.8 w\n";
    p << "250 220 m 270 220 l S\n";
    p << "250 220 m 250 240 l S\n";
    p << "350 220 m 330 220 l S\n";
    p << "350 220 m 350 240 l S\n";
    p << "250 140 m 270 140 l S\n";
    p << "250 140 m 250 120 l S\n";
    p << "350 140 m 330 140 l S\n";
    p << "350 140 m 350 120 l S\n";
    p << "300 170 m 300 190 l S\n";
    p << "290 180 m 310 180 l S\n";

    p << "BT\n";
    p << "/F3 7.5 Tf 0.5 0.6 0.7 rg 50 50 Td (ARCHIVE ID: VOL-" << info.volumeNumber << "-SEC5-P" << pageNum << " | CV SUBSYSTEM CERTIFIED) Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::generateLidarSensorChapter(const PdfVolumeInfo& info, int pageNum) {
    std::stringstream p;
    p << "q\n";
    p << "0.05 0.08 0.15 rg 0 0 612 792 re f\n";
    p << "0.0 0.7 0.9 RG 1.0 w 36 36 540 720 re S\n";

    p << "BT\n";
    p << "/F2 12 Tf 0.0 0.9 1.0 rg 50 740 Td (CHAPTER 6: 360-DEGREE LiDAR SENSORS & RELATIVISTIC RANGING) Tj\n";
    p << "/F3 8 Tf 0.6 0.7 0.8 rg 0 -13 Td (VOLUME " << info.volumeNumber << " | PARTITION B SENSORS | PAGE " << (pageNum < 10 ? "0" : "") << pageNum << " OF 40) Tj\n";

    p << "/F2 10 Tf 1.0 0.8 0.2 rg 0 -28 Td (6." << (pageNum - 22) << " CONTINUOUS FULL-SPHERICAL LASER TELEMETRY) Tj\n";
    p << "/F1 8.5 Tf 0.9 0.95 1.0 rg 0 -15 Td (The observatory suite includes a 360-degree rotating LiDAR beam scanning the entire celestial sphere) Tj\n";
    p << "0 -12 Td (at 2.8 radians per second. Laser pulses measure time-of-flight, reflectivity, and Doppler shift.) Tj\n";

    p << "/F2 9.5 Tf 0.0 1.0 0.7 rg 0 -20 Td (RELATIVISTIC TIME-OF-FLIGHT FORMULA:) Tj\n";
    p << "/F3 8.5 Tf 1.0 1.0 1.0 rg 0 -14 Td (  d = c * \\Delta t / (2 * \\sqrt{1 - (v/c)^2}) * [1 + H0 * \\Delta t / 2] ) Tj\n";
    p << "/F1 8 Tf 0.8 0.85 0.9 rg 0 -14 Td (Under extreme velocities, time dilation expands the round-trip laser pulse duration \\Delta t.) Tj\n";
    p << "0 -12 Td (The secondary factor compensates for Hubble cosmological expansion occurring during photon transit.) Tj\n";

    p << "/F2 9.5 Tf 1.0 0.6 0.2 rg 0 -22 Td (POINT-CLOUD HIT EXTRACTION & DECAY CHARACTERISTICS:) Tj\n";
    p << "/F3 8 Tf 0.0 0.9 1.0 rg 0 -14 Td (  * Pulse Repetition Rate: 120,000 pulses/second across 128 channels.) Tj\n";
    p << "0 -12 Td (  * Range Resolution      : 0.01 Gly in cosmological mode (1.2 cm in local lab mode).) Tj\n";
    p << "0 -12 Td (  * Hit Persistence Time  : Exponential decay \\tau = 1.25 seconds for visual rendering.) Tj\n";
    p << "0 -12 Td (  * Sensor Hardware       : Dual InGaAs avalanche photodiode arrays cooled to 77 Kelvin.) Tj\n";

    p << "/F2 9.5 Tf 0.0 1.0 0.6 rg 0 -22 Td (REAL-TIME INTEGRATION WITH COMPUTER VISION:) Tj\n";
    p << "/F1 8 Tf 0.8 0.85 0.9 rg 0 -13 Td (The LiDAR sweep beam in our C++ application directly intersects celestial bodies and alien presences,) Tj\n";
    p << "0 -12 Td (generating luminous green detection points that fuse with 2D CV tracking reticles.) Tj\n";
    p << "ET\n";

    // Draw LiDAR radar circle and sweep line
    p << "0.0 0.5 0.7 RG 1.0 w\n";
    p << "306 220 70 0 360 arc S\n";
    p << "306 220 40 0 360 arc S\n";
    p << "0.0 1.0 0.5 RG 1.5 w\n";
    p << "306 220 m 360 260 l S\n"; // Sweep line
    // Points
    p << "0.2 1.0 0.4 rg\n";
    p << "340 240 3 0 360 arc f\n";
    p << "325 210 2.5 0 360 arc f\n";
    p << "280 230 3 0 360 arc f\n";

    p << "BT\n";
    p << "/F3 7.5 Tf 0.5 0.6 0.7 rg 50 50 Td (ARCHIVE ID: VOL-" << info.volumeNumber << "-SEC6-P" << pageNum << " | LiDAR SYSTEM VERIFIED) Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::generateControllersChapter(const PdfVolumeInfo& info, int pageNum) {
    std::stringstream p;
    p << "q\n";
    p << "0.05 0.08 0.15 rg 0 0 612 792 re f\n";
    p << "0.0 0.7 0.9 RG 1.0 w 36 36 540 720 re S\n";

    p << "BT\n";
    p << "/F2 12 Tf 0.0 0.9 1.0 rg 50 740 Td (CHAPTER 7: CYBERNETIC ROBOT CONTROLLERS & FACIAL SYNTHESIS) Tj\n";
    p << "/F3 8 Tf 0.6 0.7 0.8 rg 0 -13 Td (VOLUME " << info.volumeNumber << " | PARTITION A ROBOTICS | PAGE " << (pageNum < 10 ? "0" : "") << pageNum << " OF 40) Tj\n";

    p << "/F2 10 Tf 1.0 0.8 0.2 rg 0 -28 Td (7." << (pageNum - 26) << " HUMAN-LIKE FACIAL EXPRESSIONS & ASYNCHRONOUS SPEECH ENGINE) Tj\n";
    p << "/F1 8.5 Tf 0.9 0.95 1.0 rg 0 -15 Td (The left partition of the observatory features humanoid robots designed with porcelain synthetic skin,) Tj\n";
    p << "0 -12 Td (dynamic iris lighting, hydraulic neck actuators, and synchronized human facial expressions:) Tj\n";

    p << "/F2 9.5 Tf 0.0 1.0 0.7 rg 0 -20 Td (PHONETIC VISEME MOUTH FORMULATIONS:) Tj\n";
    p << "/F3 8 Tf 0.0 0.9 1.0 rg 0 -14 Td (  * VISEME_REST : Resting lips (Width: 1.0, Openness: 0.0, Lip Separation: 0.0) ) Tj\n";
    p << "0 -12 Td (  * VISEME_AA   : Open oral cavity for vowel sounds 'Ah', 'Ha' (Openness: 0.65, Width: 1.05) ) Tj\n";
    p << "0 -12 Td (  * VISEME_EE   : Wide smiling lips for 'Ee', 'See' (Openness: 0.35, Width: 1.35) ) Tj\n";
    p << "0 -12 Td (  * VISEME_OH   : Rounded pursed lips for 'Oh', 'You' (Openness: 0.55, Width: 0.65) ) Tj\n";
    p << "0 -12 Td (  * VISEME_MM   : Compressed lips for bilabials 'Mm', 'B', 'P' (Openness: 0.05, Width: 0.95) ) Tj\n";

    p << "/F2 9.5 Tf 1.0 0.6 0.2 rg 0 -22 Td (MICRO-EXPRESSION & SACCADIC GAZE CONTROLLERS:) Tj\n";
    p << "/F1 8 Tf 0.8 0.85 0.9 rg 0 -13 Td (Human eyes never remain static. Our C++ FaceModel incorporates micro-saccades every 1.5 to 2.8 seconds,) Tj\n";
    p << "0 -12 Td (spontaneous blinking (fast 60ms closure, 140ms rebound), and subtle breathing sway along yaw/pitch.) Tj\n";
    p << "0 -12 Td (When speaking to you at the laptop, EVA-01 establishes direct eye contact through your webcam.) Tj\n";

    p << "/F2 9.5 Tf 0.0 0.9 1.0 rg 0 -22 Td (HARDWARE ACTUATOR PID CONTROL LOOP:) Tj\n";
    p << "/F3 8.5 Tf 1.0 1.0 1.0 rg 0 -14 Td (  u(t) = K_p * e(t) + K_i * \\int_0^t e(\\tau) d\\tau + K_d * [de(t)/dt] ) Tj\n";
    p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -14 Td (Tuned gains ensure seamless servo interpolation without robotic stepping, yielding lifelike fluidity.) Tj\n";
    p << "ET\n";

    p << "BT\n";
    p << "/F3 7.5 Tf 0.5 0.6 0.7 rg 50 50 Td (ARCHIVE ID: VOL-" << info.volumeNumber << "-SEC7-P" << pageNum << " | CONTROLLER FIRMWARE SIGNED) Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::generateQ4ExperimentChapter(const PdfVolumeInfo& info, int pageNum) {
    std::stringstream p;
    p << "q\n";
    p << "0.05 0.08 0.15 rg 0 0 612 792 re f\n";
    p << "0.0 0.7 0.9 RG 1.0 w 36 36 540 720 re S\n";

    p << "BT\n";
    p << "/F2 12 Tf 0.0 0.9 1.0 rg 50 740 Td (CHAPTER 8: QUANTUM GRAVITY Q-4 :: EXPERIMENTS & SOLUTIONS) Tj\n";
    p << "/F3 8 Tf 0.6 0.7 0.8 rg 0 -13 Td (VOLUME " << info.volumeNumber << " | PARTITION B EXPERIMENTS | PAGE " << (pageNum < 10 ? "0" : "") << pageNum << " OF 40) Tj\n";

    p << "/F2 10 Tf 1.0 0.8 0.2 rg 0 -28 Td (8." << (pageNum - 30) << " COMPLETE MATHEMATICAL RESOLUTION OF QUESTION Q-4) Tj\n";
    p << "/F1 8.5 Tf 0.9 0.95 1.0 rg 0 -15 Td (Question Q-4 addresses the fundamental boundary problem: How does quantum entanglement propagate) Tj\n";
    p << "0 -12 Td (across a cosmological horizon expanding faster than light, and what physical observables remain?) Tj\n";

    p << "/F2 9.5 Tf 0.0 1.0 0.7 rg 0 -22 Td (THE MODIFIED WHEELER-DeWITT QUANTUM GRAVITY EQUATION:) Tj\n";
    p << "/F3 8.5 Tf 1.0 1.0 1.0 rg 0 -14 Td (  \\hat{H} * \\Psi[h_{ij}, \\phi] = [ -16\\pi G * G_{ijkl} * \\frac{\\delta^2}{\\delta h_{ij} \\delta h_{kl}} - \\frac{\\sqrt{h}}{16\\pi G} * (^{(3)}R - 2\\Lambda) + \\hat{H}_m ] \\Psi = 0 ) Tj\n";
    p << "/F1 8 Tf 0.8 0.85 0.9 rg 0 -14 Td (We prove that the wave functional \\Psi does not vanish at the 93 Gly boundary. Instead, the boundary) Tj\n";
    p << "0 -12 Td (acts as an open holographic screen with non-vanishing boundary CFT correlators:) Tj\n";

    p << "/F2 9.5 Tf 1.0 0.6 0.2 rg 0 -22 Td (HOLOGRAPHIC ENTANGLEMENT ENTROPY (RYU-TAKAYANAGI GENERALIZATION):) Tj\n";
    p << "/F3 8.5 Tf 1.0 1.0 1.0 rg 0 -14 Td (  S_A = \\frac{Area(\\gamma_A)}{4 * G_N} + S_{bulk}(A) ) Tj\n";
    p << "/F1 8 Tf 0.85 0.9 0.95 rg 0 -14 Td (The bulk entropy term S_{bulk} accounts for the quantum states of the cosmic fish (Vacuum Mantas,) Tj\n";
    p << "0 -12 Td (Graviton Eels) and the Elder Alien Observer, verifying that information is rigorously preserved.) Tj\n";

    p << "/F2 9.5 Tf 0.0 0.9 1.0 rg 0 -22 Td (EXPERIMENTAL TRIAL SETUP & EMPIRICAL RESULTS:) Tj\n";
    p << "/F3 8 Tf 0.0 0.9 1.0 rg 0 -14 Td (  * Experiment Topic : " << escapePdfString(info.q4ExperimentTopic) << ") Tj\n";
    p << "0 -12 Td (  * Correlation Coeff : r = 0.99982 (Verified over 10^7 trials)) Tj\n";
    p << "0 -12 Td (  * Quantum P-Value   : p < 1.2e-14 (Empirically ruling out causal disconnection)) Tj\n";
    p << "0 -12 Td (  * Conclusion        : The unobservable universe remains causally coherent via Q-4.) Tj\n";
    p << "ET\n";

    p << "BT\n";
    p << "/F3 7.5 Tf 0.5 0.6 0.7 rg 50 50 Td (ARCHIVE ID: VOL-" << info.volumeNumber << "-SEC8-P" << pageNum << " | Q-4 SOLUTION VALIDATED) Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::generateOdometerSpeedometerChapter(const PdfVolumeInfo& info, int pageNum) {
    std::stringstream p;
    p << "q\n";
    p << "0.05 0.08 0.15 rg 0 0 612 792 re f\n";
    p << "0.0 0.7 0.9 RG 1.0 w 36 36 540 720 re S\n";

    p << "BT\n";
    p << "/F2 12 Tf 0.0 0.9 1.0 rg 50 740 Td (CHAPTER 9: COSMIC ODOMETER & RELATIVISTIC TACHOMETER LOGS) Tj\n";
    p << "/F3 8 Tf 0.6 0.7 0.8 rg 0 -13 Td (VOLUME " << info.volumeNumber << " | PARTITION B INSTRUMENTS | PAGE " << (pageNum < 10 ? "0" : "") << pageNum << " OF 40) Tj\n";

    p << "/F2 10 Tf 1.0 0.8 0.2 rg 0 -28 Td (9." << (pageNum - 35) << " SPACETIME DISTANCE AND VELOCITY METRICS) Tj\n";
    p << "/F1 8.5 Tf 0.9 0.95 1.0 rg 0 -15 Td (Robots Odos-Metric and Velox-Tachyon continuously monitor our odometer and relativistic speed dials:) Tj\n";

    // Odometer Table
    p << "/F2 9.5 Tf 0.0 1.0 0.7 rg 0 -20 Td (ODOMETER LOG TABLE (PROPER VS COMOVING DISTANCE):) Tj\n";
    p << "/F3 7.8 Tf 0.9 0.95 1.0 rg 0 -14 Td (  STEP | PROPER DIST (Gly) | COMOVING DIST (Mpc) | METRIC EXPANSION RATE | DOMAIN STATUS) Tj\n";
    p << "0 -11 Td (  -----+-------------------+---------------------+-----------------------+---------------------) Tj\n";
    p << "0 -11 Td (  001  |  16.50 Gly        |   5,061 Mpc         |  1,160 km/s           | Observable Local) Tj\n";
    p << "0 -11 Td (  002  |  32.00 Gly        |   9,812 Mpc         |  2,250 km/s           | Deep Cosmic Web) Tj\n";
    p << "0 -11 Td (  003  |  46.50 Gly        |  14,258 Mpc         |  3,270 km/s           | HORIZON BOUNDARY) Tj\n";
    p << "0 -11 Td (  004  |  48.20 Gly        |  14,779 Mpc         |  3,390 km/s (v > c)   | UNOBSERVABLE OCEAN) Tj\n";
    p << "0 -11 Td (  005  |  54.80 Gly        |  16,803 Mpc         |  3,850 km/s (v > c)   | DEEP VOID SECTOR) Tj\n";

    // Speedometer & Lorentz table
    p << "/F2 9.5 Tf 1.0 0.6 0.2 rg 0 -22 Td (RELATIVISTIC SPEEDOMETER & LORENTZ FACTOR DILATION:) Tj\n";
    p << "/F3 7.8 Tf 0.9 0.95 1.0 rg 0 -14 Td (  PRESET | FRACTION (c) | VELOCITY (km/s) | LORENTZ GAMMA \\gamma | LAPTOP CLOCK DILATION) Tj\n";
    p << "0 -11 Td (  -------+--------------+-----------------+-----------------------+----------------------) Tj\n";
    p << "0 -11 Td (  1      | 0.120 c      |   35,975 km/s   |  1.007x               | 1.007 sec/sec) Tj\n";
    p << "0 -11 Td (  2      | 0.650 c      |  194,865 km/s   |  1.316x               | 1.316 sec/sec) Tj\n";
    p << "0 -11 Td (  3      | 0.994 c      |  297,993 km/s   |  9.140x               | 9.140 sec/sec) Tj\n";
    p << "0 -11 Td (  4      | 2.450 c*     |  734,491 km/s*  | 20.825x* (WARP MODE)  | Spacetime Metric Warp) Tj\n";

    p << "/F1 8 Tf 0.8 0.85 0.9 rg 0 -16 Td (*Note: At 2.45c, local spacetime is compressed via negative energy tensors without violating local c.) Tj\n";
    p << "ET\n";

    p << "BT\n";
    p << "/F3 7.5 Tf 0.5 0.6 0.7 rg 50 50 Td (ARCHIVE ID: VOL-" << info.volumeNumber << "-SEC9-P" << pageNum << " | METROLOGY CERTIFIED) Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::generateAppendixVerificationPage(const PdfVolumeInfo& info, int pageNum) {
    std::stringstream p;
    p << "q\n";
    p << "0.05 0.08 0.15 rg 0 0 612 792 re f\n";
    p << "0.0 0.7 0.9 RG 1.0 w 36 36 540 720 re S\n";

    p << "BT\n";
    p << "/F2 12 Tf 0.0 0.9 1.0 rg 50 740 Td (CHAPTER 10: ARCHIVAL APPENDIX, CERTIFICATIONS & REFERENCES) Tj\n";
    p << "/F3 8 Tf 0.6 0.7 0.8 rg 0 -13 Td (VOLUME " << info.volumeNumber << " | FINAL VERIFICATION | PAGE " << pageNum << " OF 40) Tj\n";

    p << "/F2 10 Tf 1.0 0.8 0.2 rg 0 -28 Td (10." << (pageNum - 38) << " SYSTEM REPRODUCIBILITY & CYBERNETIC VALIDATION) Tj\n";
    p << "/F1 8.5 Tf 0.9 0.95 1.0 rg 0 -15 Td (This document completes the 40-page formal archival record for Volume " << info.volumeNumber << ". All telemetry) Tj\n";
    p << "0 -12 Td (is cross-verified by the quadruple robotic squad running live in the native C++ observatory suite.) Tj\n";

    p << "/F2 9.5 Tf 0.0 1.0 0.7 rg 0 -20 Td (CYBERNETIC SYSTEM SIGN-OFF & CRYPTOGRAPHIC TELEMETRY HASHES:) Tj\n";
    p << "/F3 7.8 Tf 0.0 0.9 1.0 rg 0 -14 Td (  * EVA-01 SIGNATURE     : SHA256: 4f8a9b2c...e810 (Ground Laptop Terminal Active)) Tj\n";
    p << "0 -12 Td (  * CHRONOS-VOID SIGN    : SHA256: 7d11f90a...b442 (248.84 GHz Satellite Stream Locked)) Tj\n";
    p << "0 -12 Td (  * THALASSA-BIO SIGN    : SHA256: 91c402e8...c771 (Cosmic Sea & Elder Alien Verified)) Tj\n";
    p << "0 -12 Td (  * VELOX-TACHYON SIGN   : SHA256: 3a778841...f098 (Lorentz Odometer & Gamma Calibrated)) Tj\n";

    p << "/F2 9.5 Tf 1.0 0.6 0.2 rg 0 -22 Td (SELECTED PRIMARY BIBLIOGRAPHY & COSMOLOGICAL SOURCES:) Tj\n";
    p << "/F1 7.8 Tf 0.85 0.9 0.95 rg 0 -13 Td (1. Einstein, A. (1916). 'Die Grundlage der allgemeinen Relativitaetstheorie.' Annalen der Physik.) Tj\n";
    p << "0 -11 Td (2. Hubble, E. (1929). 'A Relation between Distance and Radial Velocity among Extra-Galactic Nebulae.') Tj\n";
    p << "0 -11 Td (3. Wheeler, J. A., & DeWitt, B. (1967). 'Quantum Theory of Gravity. I. The Canonical Theory.') Tj\n";
    p << "0 -11 Td (4. Ryu, S., & Takayanagi, T. (2006). 'Holographic Derivation of Entanglement Entropy from AdS/CFT.') Tj\n";
    p << "0 -11 Td (5. Planck Collaboration (2020). 'Planck 2018 results. VI. Cosmological parameters.' A&A.) Tj\n";
    p << "0 -11 Td (6. Cosmic Robotics Observatory Consortium (2026). 'The Unobservable Ocean & Cybernetic Observers.') Tj\n";

    p << "/F2 9.5 Tf 0.0 0.9 1.0 rg 0 -25 Td (ARCHIVAL STATUS: COMPLETE, SEALED & IMMUTABLE [40/40 PAGES]) Tj\n";
    p << "ET\n";

    p << "BT\n";
    p << "/F3 7.5 Tf 0.5 0.6 0.7 rg 50 50 Td (ARCHIVE ID: VOL-" << info.volumeNumber << "-FINAL-P" << pageNum << " | END OF VOLUME " << info.volumeNumber << ") Tj\n";
    p << "ET\n";
    p << "Q\n";
    return p.str();
}

std::string PdfGenerator::buildPageContent(const PdfVolumeInfo& info, int pageNumber) {
    if (pageNumber == 1) {
        return generateCoverPage(info);
    } else if (pageNumber == 2) {
        return generateTOCPage(info);
    } else if (pageNumber >= 3 && pageNumber <= 6) {
        return generateOrbitSimChapter(info, pageNumber);
    } else if (pageNumber >= 7 && pageNumber <= 10) {
        return generateUnobservableOceanChapter(info, pageNumber);
    } else if (pageNumber >= 11 && pageNumber <= 14) {
        return generateCosmicFishAndAlienChapter(info, pageNumber);
    } else if (pageNumber >= 15 && pageNumber <= 18) {
        return generateCosmicFishAndAlienChapter(info, pageNumber); // Expanded alien guardian dynamics
    } else if (pageNumber >= 19 && pageNumber <= 22) {
        return generateCVAlgorithmChapter(info, pageNumber);
    } else if (pageNumber >= 23 && pageNumber <= 26) {
        return generateLidarSensorChapter(info, pageNumber);
    } else if (pageNumber >= 27 && pageNumber <= 30) {
        return generateControllersChapter(info, pageNumber);
    } else if (pageNumber >= 31 && pageNumber <= 35) {
        return generateQ4ExperimentChapter(info, pageNumber);
    } else if (pageNumber >= 36 && pageNumber <= 38) {
        return generateOdometerSpeedometerChapter(info, pageNumber);
    } else {
        return generateAppendixVerificationPage(info, pageNumber);
    }
}

bool PdfGenerator::generateSingleVolume(int volumeNumber, const std::string& outputPath) {
    PdfVolumeInfo info = getVolumeInfo(volumeNumber);

    std::ofstream pdf(outputPath, std::ios::binary);
    if (!pdf.is_open()) return false;

    // Track byte offsets for xref table
    std::vector<std::streampos> offsets;
    offsets.reserve(90);

    // Write PDF Header
    pdf << "%PDF-1.4\n%\xE2\xE3\xCF\xD3\n";

    // Object 1: Catalog
    offsets.push_back(pdf.tellp());
    pdf << "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n";

    // Object 2: Pages Tree
    offsets.push_back(pdf.tellp());
    pdf << "2 0 obj\n<< /Type /Pages /Kids [ ";
    for (int i = 0; i < 40; ++i) {
        pdf << (3 + i * 2) << " 0 R ";
    }
    pdf << "] /Count 40 >>\nendobj\n";

    // 40 Pages and their 40 Content Streams
    // For page i (0 to 39):
    // Page Obj ID = 3 + i*2
    // Stream Obj ID = 4 + i*2
    for (int i = 0; i < 40; ++i) {
        int pageObjId = 3 + i * 2;
        int streamObjId = 4 + i * 2;
        int pageNum = i + 1;

        std::string streamData = buildPageContent(info, pageNum);

        // Write Page Object
        offsets.push_back(pdf.tellp());
        pdf << pageObjId << " 0 obj\n";
        pdf << "<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792] ";
        pdf << "/Resources << /Font << /F1 83 0 R /F2 84 0 R /F3 85 0 R /F4 86 0 R >> >> ";
        pdf << "/Contents " << streamObjId << " 0 R >>\nendobj\n";

        // Write Stream Object
        offsets.push_back(pdf.tellp());
        pdf << streamObjId << " 0 obj\n";
        pdf << "<< /Length " << streamData.length() << " >>\nstream\n";
        pdf << streamData;
        pdf << "\nendstream\nendobj\n";
    }

    // Font Objects (83, 84, 85, 86)
    offsets.push_back(pdf.tellp());
    pdf << "83 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n";

    offsets.push_back(pdf.tellp());
    pdf << "84 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold >>\nendobj\n";

    offsets.push_back(pdf.tellp());
    pdf << "85 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Courier >>\nendobj\n";

    offsets.push_back(pdf.tellp());
    pdf << "86 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Courier-Bold >>\nendobj\n";

    // Cross-Reference Table (xref)
    std::streampos startXref = pdf.tellp();
    pdf << "xref\n";
    pdf << "0 87\n";
    pdf << "0000000000 65535 f \n";

    for (size_t k = 0; k < offsets.size(); ++k) {
        long off = static_cast<long>(offsets[k]);
        char buf[32];
        sprintf_s(buf, "%010ld 00000 n \n", off);
        pdf << buf;
    }

    // Trailer
    pdf << "trailer\n";
    pdf << "<< /Size 87 /Root 1 0 R >>\n";
    pdf << "startxref\n";
    pdf << static_cast<long>(startXref) << "\n";
    pdf << "%%EOF\n";

    pdf.close();
    return true;
}

bool PdfGenerator::generateAll40Volumes(const std::string& outputDir) {
    std::cout << ">>> Initializing C++ PDF Engine: Generating 40 Volumes (40 Pages Each = 1,600 Pages Total)..." << std::endl;
    for (int vol = 1; vol <= 40; ++vol) {
        char filename[64];
        sprintf_s(filename, "Cosmic_Unobservable_Treatise_Vol_%02d.pdf", vol);
        std::string fullPath = outputDir + "\\" + filename;
        
        bool ok = generateSingleVolume(vol, fullPath);
        if (ok) {
            std::cout << "  [OK] Volume " << std::setw(2) << vol << " / 40 Generated: " << filename << " (40 Pages)" << std::endl;
        } else {
            std::cerr << "  [ERROR] Failed to write Volume " << vol << std::endl;
            return false;
        }
    }
    std::cout << ">>> SUCCESS: All 40 Volumes (1,600 Pages) compiled directly in C++." << std::endl;
    return true;
}

} // namespace CosmicRobotics
