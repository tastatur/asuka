/*
CLI for predicting song genre based on the previously trained SVM model
*/
#include "utils.h"

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

    //outputToFile(predictions, audioFileName + ".yml", options);
    cout << "It's probably " << predictions.value<string>("highlevel.music.value") << " With probability " << predictions.value<Real>("highlevel.music.probability") << endl;
 
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