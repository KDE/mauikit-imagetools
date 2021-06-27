#!/bin/bash

CITIES_FILE=cities_map.h 
touch $CITIES_FILE &&

echo "#include <map>" > $CITIES_FILE
echo "using namespace std;" >> $CITIES_FILE
echo "constexpr map<string, vector<string>> cities = {" >> $CITIES_FILE

CITIES_TXT="./src/data/cities1000.txt"

if [ -f "$CITIES_TXT" ]; then
    echo "$CITIES_TXT exists."

	while read -r p; do 
		IFS=$'\t\t'
		strarr=($p)
		echo "{ \"${strarr[0]}\", {\"${strarr[1]}\", \"${strarr[4]}\", \"${strarr[5]}\", \"${strarr[8]}\", \"${strarr[17]}\"}}," >> $CITIES_FILE
		echo ${strarr[1]} 
	done < $CITIES_TXT	
echo "};" >> $CITIES_FILE

else 
    echo "$CITIES_TXT does not exist."
echo "};" >> $CITIES_FILE
	exit 0
fi