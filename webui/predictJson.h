#ifndef ASUKA_PREDICTJSON_H
#define ASUKA_PREDICTJSON_H

#include <string>
#include <iostream>
#include <essentia/essentia.h>
#include <essentia/algorithm.h>
#include <essentia/algorithmfactory.h>
#include <essentia/pool.h>
#include <cppcms/json.h>
#include "../utils.h"

using namespace std;
using namespace essentia;
using namespace essentia::standard;

void predictJsonj(const string& audioFile, const string& svmFile, const string& optionsFile);

#endif //ASUKA_PREDICTJSON_H
