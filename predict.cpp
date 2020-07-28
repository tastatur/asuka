/*
CLI for predicting song genre based on the previously trained SVM model
*/
#include "utils.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace essentia;
using namespace essentia::standard;

void predict(const string& audioFileName, const string& svmFileName, const string& profileFileName, Pool& options) {
    Pool features;
    Pool frames;

    Algorithm* extractor = AlgorithmFactory::create("MusicExtractor",
                                                    "profile", profileFileName);

    extractor->input("filename").set(audioFileName);
    extractor->output("results").set(features);
    extractor->output("resultsFrames").set(frames);   

    extractor->compute();           
    mergeValues(features, options);    

    delete extractor;   

    vector<string> svmModels = {svmFileName};
    Algorithm* predictor = AlgorithmFactory::create("MusicExtractorSVM", "svms", svmModels);

    Pool predictions;
    predictor->input("pool").set(features);
    predictor->output("pool").set(predictions);
    predictor->compute();

    cout << "It's probably " << predictions.value<string>("highlevel.music.value") << " With probability " << predictions.value<Real>("highlevel.music.probability") << endl;
 
    vector<string> genres = predictions.descriptorNames("highlevel.music.all");
    cout << "Other probabilities" << endl;
    for (auto const& genre : genres) {
        std::vector<std::string> splittedKey;
        boost::split(splittedKey, genre, [](char c){return c == '.';});

        cout << splittedKey[3] << ": " << predictions.value<Real>(genre) << endl; 
    }

    delete predictor;
}

int main(int argc, char* argv[]) {
    essentia::init();

    string audioFileName = argv[1];
    string svmFileName = argv[2];
    string profileFileName = argv[3];

    Pool options;
    setExtractorDefaultOptions(options);
    setExtractorOptions(profileFileName, options);  

    predict(audioFileName, svmFileName, profileFileName, options);

    essentia::shutdown();
}