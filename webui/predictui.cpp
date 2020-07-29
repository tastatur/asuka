#include <cppcms/application.h>  
#include <cppcms/service.h>  
#include <cppcms/http_response.h>  
#include <cppcms/url_dispatcher.h>  
#include <cppcms/url_mapper.h>  
#include <cppcms/applications_pool.h>  
#include <iostream>  
#include <cppcms/http_file.h>
#include "content.h"

class PredictUi : public cppcms::application { 
    public:

    PredictUi(cppcms::service& srv) : cppcms::application(srv) {
        dispatcher().assign("", &PredictUi::predict, this);
        mapper().assign(""); 

        mapper().root("/predict");
    }

    void predict() {
        content::predict_page c;
        if(request().request_method()=="POST") {  
            c.predict.load(context());
            c.predict.audioFile.load(context());
            c.predict.audioFile.value().get()->save_to("/tmp/test.mp3");
        }
        render("predict_page",c);
    }
};

int main(int argc, char* argv[]) {
    try {
        cppcms::service srv(argc, argv);

        srv.applications_pool().mount(cppcms::applications_factory<PredictUi>());
        srv.run();
    } catch(std::exception const &e) {  
        std::cerr << e.what() << std::endl;  
    } 
}