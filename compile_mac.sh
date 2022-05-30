#!/bin/bash
#g++ -w -Wall -D__MACOSX_CORE__ *.cpp -o bin/euclidier -framework CoreMIDI -framework coreAudio -framework CoreFoundation && ./bin/euclidier
g++ -w -Wall -D__MACOSX_CORE__ *.cpp -o bin/euclidier -framework CoreMIDI -framework coreAudio -framework CoreFoundation
