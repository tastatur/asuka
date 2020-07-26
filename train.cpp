/*
Train features extractor
*/
#include <iostream>
#include <essentia/essentia.h>
#include <essentia/algorithm.h>
#include <essentia/algorithmfactory.h> 
#include <essentia/utils/extractor_music/extractor_version.h>
#include <essentia/pool.h>
#include <filesystem>
namespace fs = std::filesystem;

using namespace essentia::standard;

int main(int argc, char* argv[]) {
    essentia::init();

    std::string input = "data/input";
    for (const auto & genreDir : fs::directory_iterator(input)) {
        std::cout << genreDir.path() << std::endl;
    }
}
