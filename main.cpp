#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <iterator>

#include "boost/filesystem.hpp"
#include "yaml-cpp/yaml.h"

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


namespace fs = boost::filesystem;

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

std::vector<std::string> getFiles(std::string path, const std::vector<std::string>& extensions={})
{
    std::vector<std::string> output;

    for (auto& entry : fs::directory_iterator(path)) {
        if (isTypeFile(entry.path().string(), extensions)) {
            output.push_back(entry.path().string());
        }
    }

    return output;
}

YAML::Node readConfiguration(const std::string& fileName)
{
    return YAML::LoadFile(fileName); 
}

void copyFile(std::string& sourcePath, std::string& destPath) 
{
    std::ifstream source{sourcePath, std::ios::binary};
    std::ofstream dest{destPath, std::ios::binary};

    std::copy(
        std::istreambuf_iterator<char>(source),
        std::istreambuf_iterator<char>(),
        std::ostreambuf_iterator<char>(dest)
    );
}

int main()
{
    // auto content = loadContent(INPUT_FILE_NAME);

    // content = str_replace(content, "html", "Dupa");

    // std::cout<<content<<std::endl<<std::flush;

    if (!fs::exists(DEST_DIRECTORY) || !fs::is_directory(DEST_DIRECTORY)) {
        throw std::runtime_error{"Destination directory does not exists"};
    }



    readConfiguration(CONFIG_FILENAME);

//    return 0;
    auto generateList = []() -> std::string {
        std::string result{};

        auto images = getFiles(DIRECTORY_PATH, {"png", "jpg"});
        std::string partialContent = loadContent(PARTIAL_TPL_PATH);


        for (auto imagePath : images) {
            result = result + str_replace(partialContent, "{$filePath}", imagePath);
        }
        
        return result;
    };

    std::string templateContent = loadContent(INDEX_TPL_PATH);
    templateContent = str_replace(templateContent, "{$imagesList}", generateList()); 

    


    std::cout<<templateContent;
}
