g++ -c main.cpp -Isrc/include

mkdir executables

g++ main.o -o executables/main.exe -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio


./executables/main.exe


