# euterpe
Music classification

## How it works

* Install essentia and gaia
* Build the project

```
$ mkdir build
$ cd build
$ cmake ../
$ make
``` 

* Prepare the input files with known genre for classification

``` 
$ bash ./prepareInput.sh /path/to/my/mp3s
``` 
* This will create directory `data/input/genre` for each genre
* You mey need to manually correct the input (e.g. remove whitespaces from genere names or combine multiple generes into one)
* Extract the features from the input audio files and train the model

``` 
$ LD_LIBRARY_PATH=/usr/local/lib ./extract ../data/input/ ../data/output/ ../extractProfile.yml 
``` 

## TODOs

* Apply the model to unknown songs
* Write some sort of REST service