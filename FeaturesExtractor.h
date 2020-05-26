#include <string>
#include <essentia/pool.h>

class FeaturesExtractor {
    public:
      essentia::Pool extract(std::string fileName);
};