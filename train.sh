#!/bin/bash

GAIA_SCRIPTS=$1
DATA_FOLDER=$(pwd)/data
mkdir -p $DATA_FOLDER/svm

PYTHONPATH=/usr/local/lib/python2.7/site-packages/ python -m gaia2.scripts.classification.train_model_from_sigs --cluster_mode $DATA_FOLDER/output $DATA_FOLDER/svm  music
