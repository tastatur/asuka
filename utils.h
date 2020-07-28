#include <essentia/essentia.h>
#include <essentia/algorithm.h>
#include <essentia/algorithmfactory.h> 
#include <essentia/pool.h>

using namespace std;
using namespace essentia;

void setExtractorDefaultOptions(Pool &options);
void setExtractorOptions(const std::string& filename, Pool& options);
void mergeValues(Pool& pool, Pool& options);
void outputToFile(Pool& pool, const string& outputFilename, Pool& options);