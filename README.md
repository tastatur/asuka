# asuka
Music classification

## How it works

* Install essentia (don't use master branch, since it has memory leaks - use release one) and gaia
* Build the project

```
$ mkdir build
$ cd build
$ cmake ../
$ make
``` 

* Prepare the train files with known genre for classification

``` 
$ bash ./prepareInput.sh /path/to/my/mp3s
``` 
* This will create directory `data/input/genre` for each genre and copy audio files to the respective folder according to ID3 genre tag.
* You mey need to manually correct the input (e.g. remove whitespaces from genere names or combine multiple generes into one)
* Extract the features from the input audio files and train the model
* This step may take a while - for a dataset containing around 1500 songs it can take around 10 hours to extract features and train a model.

``` 
$ LD_LIBRARY_PATH=/usr/local/lib ./extract ../data/input/ ../data/output/ ../extractProfile.yml 
$ bash ./train.sh
``` 

* Run predictor CLI for some unknown song to test model:

```
$ LD_LIBRARY_PATH=/usr/local/lib ./predict /tmp/Mirkka\ \ Madrugada.mp3 ../data/svm/music.history ../extractProfile.yml 
[   INFO   ] MusicExtractor: Read metadata
[   INFO   ] MusicExtractor: Compute md5 audio hash, codec, length, and EBU 128 loudness
[   INFO   ] MusicExtractor: Replay gain
[   INFO   ] MusicExtractor: Compute audio features
[   INFO   ] MusicExtractor: Compute aggregation
[   INFO   ] All done
[   INFO   ] MusicExtractorSVM: adding SVM model ../data/svm/music.history
It's probably Baiao With probability 0.470659
Other probabilities
Baiao: 0.470659
Forro: 0.424234
Samba: 0.00742601
Xaxado: 0.0596543
Xote: 0.0380265
``` 

* Build docker container and run web ui

```
$ docker build -t asuka .
$ docker run -v /tmp/asuka:/data -d -p 8080:8080 asuka
```

## TODOs

* Proper AJAX webapp