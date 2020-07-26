#1/bin/bash

INPUT=$1
OUTPUT=data/input

OIFS="$IFS"
IFS=$'\n'
for i in $(find ${INPUT} -depth -iname "*.mp3")
do
  GENRE=$(echo "$i" | perl -pe 's/^.+\(.+\)\((.+)\)(.+)$/$1/')
  DST="$OUTPUT/$GENRE/$(uuidgen).mp3"

  mkdir -p "$OUTPUT/$GENRE"
  cp "$i" "$DST"
done
IFS="$OIFS"