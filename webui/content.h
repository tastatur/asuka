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

            audioFile.non_empty();
            audioFile.add_valid_magic("\x49\x44\x33");
            audioFile.add_valid_magic("\xff\xfb");
            audioFile.add_valid_magic("\xff\xf3");
            audioFile.add_valid_magic("\xff\xf2");

            add(audioFile);
            add(submit);
        }
    };

    struct predict_page : public cppcms::base_content { 
        std::string possibleGenre;
        float probability;
        float probabilityBaiao;
        float probabilityForro;
        float probabilitySamba;
        float probabilityXaxado;
        float probabilityXote;

        predict_form predict;
    };
}

#endif