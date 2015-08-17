#!/bin/sh
g++ -c rttClient.c -o rttClient.o 
g++ rttClient.o -o rttClient -L/usr/lib/x86_64-linux-gnu/ -lboost_system -lboost_timer  `pkg-config --libs --cflags libmodbus`
./rttClient 127.0.0.1



