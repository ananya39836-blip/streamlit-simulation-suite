#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

namespace CosmicRobotics {

struct PdfVolumeInfo {
    int volumeNumber;
    std::string volumeTitle;
    std::string subtitle;
    std::string primaryDomain;
    std::string focusPhenomenon;
    std::string q4ExperimentTopic;
    std::string cvLidarSensorSystem;
};

class PdfGenerator {
public:
    PdfGenerator();
    ~PdfGenerator();

    // Generates all 40 PDFs, each of 40 pages = 1600 pages total in output directory
    static bool generateAll40Volumes(const std::string& outputDir);

    // Generates a single 40-page PDF document
    static bool generateSingleVolume(int volumeNumber, const std::string& outputPath);

private:
    static std::string buildPageContent(const PdfVolumeInfo& info, int pageNumber);
    static void writePageStream(std::stringstream& ss, const std::string& streamContent);
    static PdfVolumeInfo getVolumeInfo(int volumeNumber);

    // Page generators for specific sections within the 40 pages
    static std::string generateCoverPage(const PdfVolumeInfo& info);
    static std::string generateTOCPage(const PdfVolumeInfo& info);
    static std::string generateOrbitSimChapter(const PdfVolumeInfo& info, int pageNum);
    static std::string generateUnobservableOceanChapter(const PdfVolumeInfo& info, int pageNum);
    static std::string generateCosmicFishAndAlienChapter(const PdfVolumeInfo& info, int pageNum);
    static std::string generateCVAlgorithmChapter(const PdfVolumeInfo& info, int pageNum);
    static std::string generateLidarSensorChapter(const PdfVolumeInfo& info, int pageNum);
    static std::string generateControllersChapter(const PdfVolumeInfo& info, int pageNum);
    static std::string generateQ4ExperimentChapter(const PdfVolumeInfo& info, int pageNum);
    static std::string generateOdometerSpeedometerChapter(const PdfVolumeInfo& info, int pageNum);
    static std::string generateAppendixVerificationPage(const PdfVolumeInfo& info, int pageNum);
};

} // namespace CosmicRobotics
