#include "ObservatoryWindow.hpp"
#include "PdfGenerator.hpp"
#include <iostream>
#include <direct.h>
#include <thread>

using namespace CosmicRobotics;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Ensure treatises output directory exists
    std::string pdfOutputDir = "C:\\Users\\HP\\.gemini\\antigravity\\scratch\\cosmic_robotics_observatory\\treatises_40_volumes";
    _mkdir(pdfOutputDir.c_str());

    // Generate all 40 PDF volumes (each of 40 pages = 1600 pages total) in background or foreground
    // Launching in a worker thread so the GUI window opens immediately without delay
    std::thread pdfThread([pdfOutputDir]() {
        PdfGenerator::generateAll40Volumes(pdfOutputDir);
    });
    pdfThread.detach();

    // Launch the interactive C++ GUI window
    ObservatoryWindow window;
    if (!window.initialize(hInstance, 1480, 900)) {
        MessageBoxW(NULL, L"Failed to initialize Cosmic Robotics Observatory Window.", L"Error", MB_ICONERROR);
        return 1;
    }

    window.run();

    return 0;
}

// Console entry point for testing and direct CLI execution
int main(int argc, char* argv[]) {
    std::string pdfOutputDir = "C:\\Users\\HP\\.gemini\\antigravity\\scratch\\cosmic_robotics_observatory\\treatises_40_volumes";
    _mkdir(pdfOutputDir.c_str());

    if (argc > 1 && std::string(argv[1]) == "--generate-pdfs") {
        std::cout << "=================================================================" << std::endl;
        std::cout << "COSMIC ROBOTICS OBSERVATORY :: 40-VOLUME 40-PAGE C++ PDF COMPILER" << std::endl;
        std::cout << "=================================================================" << std::endl;
        bool ok = PdfGenerator::generateAll40Volumes(pdfOutputDir);
        return ok ? 0 : 1;
    }

    HINSTANCE hInstance = GetModuleHandle(NULL);
    return WinMain(hInstance, NULL, GetCommandLineA(), SW_SHOW);
}
