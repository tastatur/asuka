#include <cppcms/application.h>  
#include <cppcms/service.h>  
#include <cppcms/http_response.h>  
#include <cppcms/url_dispatcher.h>  
#include <cppcms/url_mapper.h>  
#include <cppcms/applications_pool.h>  
#include <iostream>  
#include <cppcms/http_file.h>
#include <filesystem>
#include <random>
#include <string>
#include "content.h"
#include "../utils.h"

using namespace std;
using namespace essentia;
using namespace essentia::standard;

class PredictUi : public cppcms::application { 
    private:
      Pool options;
      string profileFileName;
      string svmPath;
      string tempPath;

    string random_string(int size) {
        string chars("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        random_device rd;
        mt19937 generator(rd());

        std::shuffle(chars.begin(), chars.end(), generator);
        return chars.substr(0, size); 
    }
   
    public:

    PredictUi(cppcms::service& srv) : cppcms::application(srv) {
        dispatcher().assign("", &PredictUi::predict, this);
        mapper().assign(""); 

        mapper().root("/predict");
        cppcms::json::value settings = srv.settings();

        profileFileName = settings["profile"].str();
        svmPath = settings["svmPath"].str();
        tempPath = settings["tempPath"].str();

        setExtractorDefaultOptions(options);
        setExtractorOptions(profileFileName, options);  
    }

    void predict() {
        content::predict_page c;
        if(request().request_method() == "POST") {  
            c.predict.load(context());
            if (c.predict.validate()) {     
                cppcms::http::file* audioFile = c.predict.audioFile.value().get();
                
                //std::string filename = string(random_string(31) + ".mp3");
                std::string filename = string(tempPath + random_string(31) + ".mp3");
                audioFile->save_to(filename);

                Pool features;
                Pool frames;

                Algorithm* extractor = AlgorithmFactory::create("MusicExtractor",
                                                    "profile", profileFileName);

                extractor->input("filename").set(filename);
                extractor->output("results").set(features);
                extractor->output("resultsFrames").set(frames);   
                extractor->compute();      

                mergeValues(features, options);    

                delete extractor;  

                vector<string> svmModels = {svmPath};
                Algorithm* predictor = AlgorithmFactory::create("MusicExtractorSVM", "svms", svmModels);
                Pool predictions;
                predictor->input("pool").set(features);
                predictor->output("pool").set(predictions);
                predictor->compute();

                c.possibleGenre = predictions.value<string>("highlevel.music.value");

                delete predictor; 

                audioFile->close();                
                c.predict.clear();

                filesystem::remove(filename);
            }
        }
        render("predict_page",c);
    }
};

int main(int argc, char* argv[]) {
    try {
        cppcms::service srv(argc, argv);

        essentia::init();
        essentia::warningLevelActive = false; // deactivate warnings
        essentia::infoLevelActive = false;    // deactivate info
        essentia::errorLevelActive = false;    // activate error level
        //essentia::setDebugLevel(EAll);

        srv.applications_pool().mount(cppcms::applications_factory<PredictUi>());
        srv.run();
    } catch(std::exception const &e) {  
        std::cerr << e.what() << std::endl;  
    } 

    essentia::shutdown();
}