#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <iterator>
#include "Config.hpp"
#include "boost/filesystem.hpp"
#include <Magick++.h>

#define INPUT_FILE_NAME "test.html"
#define PARTIAL_TPL_PATH "templates/partial.html"
#define INDEX_TPL_PATH "templates/index.html"
#define DIRECTORY_PATH "test"
#define CONFIG_FILENAME "config.yml"
#define TPL_DIRECTORY "templates/"
#define DEST_DIRECTORY "/var/www/html/gallery/"
std::vector<std::string> filesToCopy{
    "fluid-gallery.css"
};





std::string str_replace(std::string str, const std::string& from, const std::string& to)
{
    size_t startPos = 0;

    while((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
    }

    return str;
}

std::string loadContent(const std::string& filePath)
{
    std::ifstream fileStreamHandle(filePath);

    return std::string(std::istreambuf_iterator<char>(fileStreamHandle),
                        (std::istreambuf_iterator<char>()));
}

bool isTypeFile(const std::string& filePath, const std::vector<std::string>& extensions)
{
    for (const auto& ext : extensions) {
        if (std::string::npos != filePath.find(ext, filePath.length()-ext.length()-1)) {
            return true;
        }
    }

    return false;
}

std::vector<fs::path> getFiles(const std::vector<std::string>& paths, const std::vector<std::string>& extensions={})
{
    std::vector<fs::path> output;

    for (auto& path : paths) {
        for (auto& entry : fs::directory_iterator(path)) {
            if (isTypeFile(entry.path().string(), extensions)) {
                output.push_back(entry.path());
            }
        }
    }

    return output;
}

void copyFile(const std::string& sourcePath, const std::string& destPath)
{
    std::ifstream source{sourcePath, std::ios::binary};
    std::ofstream dest{destPath, std::ios::binary};

    std::copy(
        std::istreambuf_iterator<char>(source),
        std::istreambuf_iterator<char>(),
        std::ostreambuf_iterator<char>(dest)
    );
}

void writeFile(const std::string& destFile, const std::string& content)
{
    std::ofstream{destFile}<<content;
}

void scaleImage(const fs::path& path, const std::string& outDir, uint32_t w, uint32_t h)
{
    Magick::Image image;

    image.read(path.string());
    image.resize(Magick::Geometry{w, h});

    auto geometry = image.size();

    if (geometry.width() < geometry.height()) {
        int yOffset = (geometry.height()-geometry.width()) / 2;
        geometry = Magick::Geometry{
            geometry.width(),
            geometry.width(),
            0,
            yOffset
        };
    } else {
        int xOffset = ((geometry.width()-geometry.height()) / 2);
        geometry = Magick::Geometry{
            geometry.height(),
            geometry.height(),
            xOffset,
            0
        };
    }

    image.crop(geometry);

    image.write(outDir + "/" + path.filename().string());
}

void createThumbnails(const std::vector<fs::path>& images, const Config& conf)
{

    for (auto& image : images) {
        scaleImage(
            image,
            conf.getDestDir() + "/" + conf.getThumbnailsDir(),
            conf.getThumbnailSize().width,
            conf.getThumbnailSize().height
        );
    }

}

int main()
{
    Config conf{"config.yml"};

    auto images = getFiles(conf.getPhotoSourceDirs(), {"png", "jpg"});

    if (!fs::exists(conf.getDestDir() + "/" + conf.getThumbnailsDir())) {
        fs::create_directory(conf.getDestDir() + "/" + conf.getThumbnailsDir());
    }

    createThumbnails(images, conf);

    auto generateList = [&]() -> std::string {
        std::string result{};

        std::string partialContent = loadContent(PARTIAL_TPL_PATH);

        for (auto& imagePath : images) {
            partialContent = str_replace(partialContent, "{$filePath}", imagePath.string());
            result = result + str_replace(
                partialContent,
                "{$thumbnailPath}",
                conf.getDestDir() + "/" + conf.getThumbnailsDir() + "/" + imagePath.filename().string()
            );
        }

        return result;
    };

    std::string templateContent = loadContent(INDEX_TPL_PATH);

    templateContent = str_replace(templateContent, "{$imagesList}", generateList());

    writeFile(conf.getDestDir()+"/index.html", templateContent);

    for (auto& file : conf.getTemplateFiles()) {
        copyFile(conf.getSourceDir() + "/" + file, conf.getDestDir() + "/" + file);
    }

}
//dominikzaq