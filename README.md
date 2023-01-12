# Championship Project

## About

This project was made for my Computer Networks course. I created a client-server app using multithreading in C that administrates different championships.

## Description
Create a client/server application that will manage different championships. The server application will have the following functionalities: user registration (different types of users: regular, administrators). All orders will be restricted by the login section. After logging in, users will be able to receive information about the last registered championships, having the possibility to register for them. The commands that must be implemented: registration of a championship, specifying the game, the number of players, different rules or structures of the championship (single-elimination, double elimination), the method of drawing games (deciding the games), registering a user in a championship, the user he will be informed via e-mail if he has been accepted in the respective championship, and he will receive additional information about his games (time, opponent, etc...) The user has the possibility to reschedule a game session. Admin users will have a history of match scores.

## Commands

* login
* logout
* create account
* edit account
* show championships
* history
* create championship
* edit championship
* participate
* quit

## Technologies
The project is created with:
* C programming language
* Sqlite database

## Setup
To run this project, install this on your local device:
 sudo apt-get install libcurl4-nss-dev
 
 ```
$ gcc baze.c -o baze -lsqlite3
$ ./baze
$ make
$ ./client
```
