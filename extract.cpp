/*
Extract features from songs
*/
#include <iostream>
#include <thread>
#include <filesystem>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio.hpp>
#include "utils.h"

namespace fs = std::filesystem;

using namespace essentia;
using namespace std;
using namespace essentia::standard;

void computeFearures(const string& inputFile, const string& outputFile, const string& profileFilename) {
    Pool options;

    setExtractorDefaultOptions(options);
    setExtractorOptions(profileFilename, options);    

    Algorithm* extractor = AlgorithmFactory::create("MusicExtractor",
                                                    "profile", profileFilename);

    Pool results;
    Pool resultsFrames;

    extractor->input("filename").set(inputFile);
    extractor->output("results").set(results);
    extractor->output("resultsFrames").set(resultsFrames);   

    extractor->compute();           
    mergeValues(results, options);


    outputToFile(results, outputFile, options);    
    if (options.value<Real>("outputFrames")) {
      outputToFile(resultsFrames, outputFile+"_frames", options);
    }

    delete extractor;
}

int main(int argc, char* argv[]) {
    essentia::init();
    essentia::warningLevelActive = false; // deactivate warnings
    essentia::infoLevelActive = false;    // deactivate info
    essentia::errorLevelActive = false;    // activate error level

    std::string inputDir = argv[1];
    std::string outputDir = argv[2];
    std::string profile = argv[3];    
    
    boost::asio::thread_pool pool(4);
    for (const auto & genreDir : fs::directory_iterator(inputDir)) {        
        std::string genre = genreDir.path().stem();
        fs::create_directory(outputDir + genre);

        for (const auto & audioFile : fs::directory_iterator(genreDir.path())) {
            const auto inputFile = audioFile.path();    
            std::string inputFileName = inputFile.stem();

            //computeFearures(inputFile, outputDir + genre + "/" + inputFileName + ".sig",  profile);
            boost::asio::post(pool,
              [outputDir, inputFile, genre, profile, inputFileName]()
                 {
                      computeFearures(inputFile, outputDir + genre + "/" + inputFileName + ".sig",  profile);
                 });
        }                  
    }
    pool.join();
    essentia::shutdown();
}
