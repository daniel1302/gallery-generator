#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "Config.hpp"
#include "boost/filesystem.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <Magick++.h>

namespace fs = boost::filesystem;

class Generator
{
private:
    Config config;
    std::vector<fs::path> images{};
    bool isFileTypeSupported();

    void getImages();
    void createThumbnails();
    void scaleAndCropImage(const fs::path& path);
    bool isFileTypeSupported(const std::string& filePath, const std::vector<std::string>& extensions);
    std::string getHtmlImageList();
    std::string loadContent(const std::string& filePath);
    std::string str_replace(std::string str, const std::string& from, const std::string& to);
    void copyFile(const std::string& sourcePath, const std::string& destPath);
    void writeFile(const std::string& destFile, const std::string& content);


public:
    Generator(const std::string configPath);
    void setVerbose(bool status);
    void generate();
};


#endif