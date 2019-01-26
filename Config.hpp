#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "yaml-cpp/yaml.h"
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;


class Config
{
private:
    struct ImageSize
    {
        uint32_t width = 100;
        uint32_t height = 100;

        ImageSize(uint32_t w=0, uint32_t h=0) {
            if (w>0) width = w;
            if (h>0) height = h;
        }
    };

    void parseConfigFile();
    void validateConfigs();
    void validateFileExisting(const std::string& path);

    YAML::Node config;
    std::string sourceDir;
    std::string indexFile;
    std::string destDir;
    std::string thumbnailsDir;
    std::vector<std::string> templateFiles{};
    std::vector<std::string> photoSourceDirs{};
    std::vector<std::string> allowedExtensions{};
    ImageSize thumbnailSize{};

public:
    Config(const std::string& filePath);

    std::string getSourceDir() const;
    std::string getIndexFile() const;
    std::string getDestDir() const;
    std::string getThumbnailsDir() const;
    std::vector<std::string> getTemplateFiles() const;
    std::vector<std::string> getPhotoSourceDirs() const;
    std::vector<std::string> getAllowedExtensions() const;
    ImageSize getThumbnailSize() const;
};

#endif