/*
Extract features from songs
*/
#include <iostream>
#include <thread>
#include <essentia/essentia.h>
#include <essentia/algorithm.h>
#include <essentia/algorithmfactory.h> 
#include <essentia/utils/extractor_music/extractor_version.h>
#include <essentia/pool.h>
#include <filesystem>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio.hpp>

namespace fs = std::filesystem;

using namespace essentia;
using namespace std;
using namespace essentia::standard;

void setExtractorDefaultOptions(Pool &options) {
  options.set("outputFrames", false);
  options.set("outputFormat", "yaml");
  options.set("indent", 4);
  
  options.set("lowlevel.stats", "mean, var, median, min, max, dmean, dmean2, dvar, dvar2");
  options.set("rhythm.stats", "mean, var, median, min, max, dmean, dmean2, dvar, dvar2");
  options.set("tonal.stats", "mean, var, median, min, max, dmean, dmean2, dvar, dvar2");
}

void setExtractorOptions(const std::string& filename, Pool& options) {
  if (filename.empty()) return;

  Pool opts;
  Algorithm * yaml = AlgorithmFactory::create("YamlInput", "filename", filename);
  yaml->output("pool").set(opts);
  yaml->compute();
  delete yaml;

  options.merge(opts, "replace");
}

void mergeValues(Pool& pool, Pool& options) {
  string mergeKeyPrefix = "mergeValues";
  vector<string> keys = options.descriptorNames(mergeKeyPrefix);

  for (int i=0; i<(int) keys.size(); ++i) {
    keys[i].replace(0, mergeKeyPrefix.size()+1, "");
    pool.set(keys[i], options.value<string>(mergeKeyPrefix + "." + keys[i]));
  }
}

void outputToFile(Pool& pool, const string& outputFilename, Pool& options) {

  int indent = (int)options.value<Real>("indent");

  string format = options.value<string>("outputFormat");
  Algorithm* output = AlgorithmFactory::create("YamlOutput",
                                               "filename", outputFilename,
                                               "doubleCheck", true,
                                               "format", format,
                                               "writeVersion", false,
                                               "indent", indent);
  output->input("pool").set(pool);
  output->compute();
  delete output;
}

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
