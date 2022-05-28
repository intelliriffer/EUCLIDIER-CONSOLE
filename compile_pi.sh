#!/bin/bash
#g++ -w -Wall -D__UNIX_JACK__ *.cpp -o seq -ljack  && ./seq

#g++ -w -Wall -D__LINUX_ALSA__ -O3 -fPIC -Wno-unused-variable  *.cpp -o euclidier  -lncurses -lm -ldl -lstdc++ -lasound -lpthread  && ./euclidier
g++ -w -Wall -D__LINUX_ALSA__ -O3 -fPIC -Wno-unused-variable *.cpp -o bin/euclidier -lncurses -lm -ldl -lstdc++ -lasound -lpthread
