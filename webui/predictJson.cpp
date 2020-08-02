#include "predictJson.h"

void predictJsonj(const string& audioFile, const string& svmFile, const string& optionsFile) {
    essentia::init();
    essentia::warningLevelActive = false; // deactivate warnings
    essentia::infoLevelActive = false;    // deactivate info
    essentia::errorLevelActive = false;    // activate error level

    Pool options;
    setExtractorDefaultOptions(options);
    setExtractorOptions(optionsFile, options);

    Pool features;
    Pool frames;

    Algorithm *extractor = AlgorithmFactory::create("MusicExtractor",
                                                    "profile", optionsFile);

    extractor->input("filename").set(audioFile);
    extractor->output("results").set(features);
    extractor->output("resultsFrames").set(frames);
    extractor->compute();

    mergeValues(features, options);
    delete extractor;

    vector<string> svmModels = {svmFile};
    Algorithm *predictor = AlgorithmFactory::create("MusicExtractorSVM", "svms", svmModels);
    Pool predictions;
    predictor->input("pool").set(features);
    predictor->output("pool").set(predictions);
    predictor->compute();

    cppcms::json::value data;
    data["value"] = predictions.value<string>("highlevel.music.value");
    data["probability"] = predictions.value<Real>("highlevel.music.probability");
    data["probabilityBaiao"] = predictions.value<Real>("highlevel.music.all.Baiao");
    data["probabilityForro"] = predictions.value<Real>("highlevel.music.all.Forro");
    data["probabilitySamba"] = predictions.value<Real>("highlevel.music.all.Samba");
    data["probabilityXaxado"] = predictions.value<Real>("highlevel.music.all.Xaxado");
    data["probabilityXote"] = predictions.value<Real>("highlevel.music.all.Xote");

    cout << data.save() << endl;

    delete predictor;
    essentia::shutdown();
}