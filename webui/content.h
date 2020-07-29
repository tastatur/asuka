#ifndef CONTENT_H
#define CONTENT_H

#include <cppcms/view.h>
#include <cppcms/form.h>
#include <iostream>

namespace content  {
    struct predict_form : public cppcms::form {
        cppcms::widgets::file audioFile;
        cppcms::widgets::submit submit;

        predict_form() {
            submit.value("Send");
            add(audioFile);
            add(submit);
        }
    };

    struct predict_page : public cppcms::base_content { 
        std::string possibleGenre;
        predict_form predict;
    };
}

#endif