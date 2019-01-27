#include "Generator.hpp"


Generator::Generator(const std::string configPath)
    : config(Config{configPath})
{
    Magick::InitializeMagick("");
}

void Generator::generate()
{
    getImages();
    if (config.getThumbnailEnabled()) {
        createThumbnails();
    }

    std::string templateContent = loadContent(config.getSourceDir() + "/" + config.getIndexFile());

    templateContent = str_replace(templateContent, "{$imagesList}", getHtmlImageList());

    templateContent = str_replace(templateContent, "{$title}", config.getTitle());
    templateContent = str_replace(templateContent, "{$description}", config.getDescription());

    writeFile(config.getDestDir()+"/index.html", templateContent);

    for (auto& file : config.getTemplateFiles()) {
        copyFile(config.getSourceDir() + "/" + file, config.getDestDir() + "/" + file);
    }

}

void Generator::createThumbnails()
{
    for (auto& image : images) {
        scaleAndCropImage(image);
    }
}

void Generator::scaleAndCropImage(const fs::path& path)
{
    if (!config.getThumbnailEnabled()) {
        return;
    }

    Magick::Image image;

    image.read(path.string());
    image.resize(Magick::Geometry{
        config.getThumbnailSize().width,
        config.getThumbnailSize().height,
    });

    if (config.getThumbnailCrop()) {
        auto geometry = image.size();

        if (geometry.width() < geometry.height()) {
            uint32_t yOffset = (geometry.height()-geometry.width()) / 2;
            geometry = Magick::Geometry{
                geometry.width(),
                geometry.width(),
                0,
                yOffset
            };
        } else {
            uint32_t xOffset = ((geometry.width()-geometry.height()) / 2);
            geometry = Magick::Geometry{
                geometry.height(),
                geometry.height(),
                xOffset,
                0
            };
        }

        image.crop(geometry);
    }

    image.write(config.getDestDir() + "/" + config.getThumbnailsDir() + "/" + path.filename().string());
}

bool Generator::isFileTypeSupported(const std::string& filePath, const std::vector<std::string>& extensions)
{
    for (const auto& ext : extensions) {
        if (std::string::npos != filePath.find(ext, filePath.length()-ext.length()-1)) {
            return true;
        }
    }

    return false;
}

void Generator::getImages()
{
    for (auto& path : config.getPhotoSourceDirs()) {
        for (auto& entry : fs::directory_iterator(path)) {
            if (!isFileTypeSupported(entry.path().string(), config.getAllowedExtensions())) {
                continue;
            }

            images.push_back(entry.path());
        }
    }
}

std::string Generator::getHtmlImageList()
{
    std::string result{};

    std::string partialContent = loadContent(config.getSourceDir() + "/" + config.getPartialFile());

    for (auto& imagePath : images) {

        result = result + str_replace(
            str_replace(
                partialContent,
                "{$filePath}",
                imagePath.string()
            ),
            "{$thumbnailPath}",
            config.getDestDir() + "/" + config.getThumbnailsDir() + "/" + imagePath.filename().string()
        );
    }

    return result;
}

std::string Generator::loadContent(const std::string& filePath)
{
    std::ifstream fileStreamHandle(filePath);

    return std::string(std::istreambuf_iterator<char>(fileStreamHandle),
                        (std::istreambuf_iterator<char>()));
}

std::string Generator::str_replace(std::string str, const std::string& from, const std::string& to)
{
    size_t startPos = 0;

    while((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
    }

    return str;
}

void Generator::copyFile(const std::string& sourcePath, const std::string& destPath)
{
    std::ifstream source{sourcePath, std::ios::binary};
    std::ofstream dest{destPath, std::ios::binary};

    std::copy(
        std::istreambuf_iterator<char>(source),
        std::istreambuf_iterator<char>(),
        std::ostreambuf_iterator<char>(dest)
    );
}

void Generator::writeFile(const std::string& destFile, const std::string& content)
{
    std::ofstream{destFile}<<content;
}