g++ -Wall -std=gnu++11 -c main.cc -I/usr/lib/x86_64-linux-gnu/include
g++ main.o -O3 -o chess -L/usr/lib/x86_64-linux-gnu/lib -lsfml-graphics -lsfml-window -lsfml-system
