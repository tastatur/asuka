#1/bin/bash

INPUT=$1
OUTPUT=data/input

getGenre() { ffprobe "$1" 2>&1 | sed -E -n 's/^ *genre *: (.*)/\1/p'; }

OIFS="$IFS"
IFS=$'\n'
for i in $(find ${INPUT} -depth -iname "*.mp3")
do
  GENRE="$(getGenre $i | tr ' ' '_')"
  DST="$OUTPUT/$GENRE/$(uuidgen).mp3"

  mkdir -p "$OUTPUT/$GENRE"
  cp "$i" "$DST"
done
IFS="$OIFS"