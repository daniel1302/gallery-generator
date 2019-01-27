#include "Config.hpp"

#include <iostream>


Config::Config(const std::string& filePath)
{
    if (!fs::exists(filePath)) {
        throw std::runtime_error{"Config file does not exists"};
    }

    config = YAML::LoadFile(filePath);

    parseConfigFile();
    validateConfigs();
}

void Config::parseConfigFile()
{
    auto appendSequence = [](std::vector<std::string>& vec, const YAML::Node& node)
    {
        if (!node || !node.IsSequence() || node.size() < 1) {
            return;
        }

        for (auto& item : node) {
            vec.push_back(item.as<std::string>());
        }
    };

    sourceDir = config["template"]["source_dir"].as<std::string>();
    indexFile = config["template"]["index_file"].as<std::string>();
    title = config["template"]["title"].as<std::string>();
    description = config["template"]["description"].as<std::string>();
    partialFile = config["template"]["partial_file"].as<std::string>();
    destDir = config["template"]["dest_dir"].as<std::string>();
    thumbnailsDir = config["thumbnails"]["dir"].as<std::string>();
    thumbnailEnabled = config["thumbnails"]["enabled"].as<bool>();
    thumbnailCrop = config["thumbnails"]["crop"].as<bool>();

    thumbnailSize = Config::ImageSize{
        config["thumbnails"]["size"]["width"].as<uint32_t>(),
        config["thumbnails"]["size"]["height"].as<uint32_t>()
    };



    appendSequence(templateFiles, config["template"]["template_files"]);
    appendSequence(photoSourceDirs, config["photo_source_dirs"]);
    appendSequence(allowedExtensions, config["allowed_extensions"]);

}

void Config::validateFileExisting(const std::string& path)
{
    if (fs::exists(path)) {
        return;
    }

    throw std::runtime_error{"File or directory " + path + " does not exist"};
}

void Config::validateConfigs()
{
    if (sourceDir.length() < 1
        || indexFile.length() < 1
        || destDir.length() < 1
        || photoSourceDirs.size() < 1
    ) {
        throw std::runtime_error{"Missing config keys"};
    }

    validateFileExisting(sourceDir);
    validateFileExisting(sourceDir + "/" + indexFile);
    validateFileExisting(sourceDir + "/" + partialFile);

    for (auto& file : templateFiles) {
        validateFileExisting(sourceDir + "/" + file);
    }

    for (auto& file : photoSourceDirs) {
        validateFileExisting(file);
    }

    if (fs::exists(destDir + "/" + thumbnailsDir) && !fs::is_directory(destDir + "/" + thumbnailsDir)) {
        throw std::runtime_error{destDir + "/" + thumbnailsDir + " exists and this is not directory"};
    }

}

std::string Config::getSourceDir() const
{
    return sourceDir;
}

std::string Config::getIndexFile() const
{
    return indexFile;
}

std::string Config::getTitle() const
{
    return title;
}

std::string Config::getDescription() const
{
    return description;
}

std::string Config::getPartialFile() const
{
    return partialFile;
}

std::string Config::getDestDir() const
{
    return destDir;
}

std::string Config::getThumbnailsDir() const
{
    return thumbnailsDir;
}

std::vector<std::string> Config::getTemplateFiles() const
{
    return templateFiles;
}

std::vector<std::string> Config::getPhotoSourceDirs() const
{
    return photoSourceDirs;
}

std::vector<std::string> Config::getAllowedExtensions() const
{
    return allowedExtensions;
}

Config::ImageSize Config::getThumbnailSize() const
{
    return thumbnailSize;
}

bool Config::getThumbnailEnabled() const
{
    return thumbnailEnabled;
}

bool Config::getThumbnailCrop() const
{
    return thumbnailCrop;
}