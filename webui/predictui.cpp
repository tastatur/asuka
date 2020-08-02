#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/applications_pool.h>
#include <iostream>
#include <cppcms/http_file.h>
#include <filesystem>
#include <random>
#include <boost/process.hpp>
#include <string>
#include "content.h"
#include "predictJson.h"

using namespace std;

class PredictUi : public cppcms::application {
private:
    string profileFileName;
    string svmPath;
    string tempPath;

    string random_string(int size) {
        string chars("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        random_device rd;
        mt19937 generator(rd());

        std::shuffle(chars.begin(), chars.end(), generator);
        return chars.substr(0, size);
    };

    vector<std::string> executePredict(std::string& audioFile) {
        boost::process::ipstream is;
        boost::process::child c(boost::process::search_path("webui"), "predict", audioFile, svmPath, profileFileName, boost::process::std_out > is);

        std::vector<std::string> data;
        std::string line;

        while (c.running() && std::getline(is, line) && !line.empty())
            data.push_back(line);

        c.wait();

        return data;
    }

public:
    PredictUi(cppcms::service &srv) : cppcms::application(srv) {
        dispatcher().assign("", &PredictUi::predict, this);
        mapper().assign("");

        mapper().root("/predict");
        cppcms::json::value settings = srv.settings();

        profileFileName = settings["profile"].str();
        svmPath = settings["svmPath"].str();
        tempPath = settings["tempPath"].str();
    }

    void predict() {
        content::predict_page c;
        if (request().request_method() == "POST") {
            c.predict.load(context());
            if (c.predict.validate()) {
                cppcms::http::file *audioFile = c.predict.audioFile.value().get();

                std::string filename = string(tempPath + random_string(31) + ".mp3");
                audioFile->save_to(filename);
                audioFile->close();

                std::vector<std::string> predictionsData = executePredict(filename);
                istringstream predictionsStream(predictionsData[0]);
                cppcms::json::value predictions;
                predictions.load(predictionsStream, true);

                c.possibleGenre = predictions["value"].str();
                c.probability = predictions["probability"].get_value<Real>();
                c.probabilityBaiao = predictions["probabilityBaiao"].get_value<Real>();
                c.probabilityForro = predictions["probabilityForro"].get_value<Real>();
                c.probabilitySamba = predictions["probabilitySamba"].get_value<Real>();
                c.probabilityXaxado = predictions["probabilityXaxado"].get_value<Real>();
                c.probabilityXote = predictions["probabilityXote"].get_value<Real>();

                c.predict.clear();
                filesystem::remove(filename);
            }
        }
        render("predict_page", c);
    }
};

int main(int argc, char *argv[]) {

    if (strcmp("predict", argv[1]) == 0) {
        string audioFile = string(argv[2]);
        string svmFile = string(argv[3]);
        string optionsFile = string(argv[4]);

        predictJsonj(audioFile, svmFile, optionsFile);

        return 0;
    }

    try {
        cppcms::service srv(argc, argv);
        srv.applications_pool().mount(cppcms::applications_factory<PredictUi>());
        srv.run();
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}