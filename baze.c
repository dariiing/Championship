#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   return 0;
}

int main(int argc, char* argv[]) {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open("test.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stdout, "Opened database successfully\n");
   }

   /* Create SQL statement */

   sql = "CREATE TABLE ACCOUNTS(" \
      "USERNAME CHAR(10) PRIMARY KEY NOT NULL,"\
      "PASSWORD TEXT NOT NULL,"\
      "NAME TEXT NOT NULL,"\
      "ADMIN INT NOT NULL,"\
      "STRONG TEXT,"\
      "WEAK TEXT,"\
      "EMAIL TEXT NOT NULL);";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      printf("SQL error: %s\n", zErrMsg);
   } else {
      printf("Table created successfully\n");
   }

   sql = "CREATE TABLE CHAMPIONSHIPS(" \
      "NAME TEXT PRIMARY KEY NOT NULL,"\
      "TYPE TEXT NOT NULL,"\
      "NB_PLAYERS INT NOT NULL,"\
      "NR_PARTICIPANTI INT,"\
      "PARTICIPANTI TEXT,"\
      "STRUCTURE TEXT NOT NULL,"\
      "HISTORY TEXT,"\
      "WINNER TEXT,"\
      "GAMES DATE,"\
      "ORA TEXT,"\
      "LOCATION TEXT,"\
      "DESC TEXT);";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      printf("SQL error: %s\n", zErrMsg);
   } else {
      printf("Table created successfully\n");
   }

   sql = "INSERT INTO ACCOUNTS (USERNAME,PASSWORD, NAME,ADMIN,STRONG, WEAK, EMAIL) "  \
         "VALUES ('daria','daria', 'Daria', 1,'tennis','football',  'dariae9@yahoo.com'); " \
         "INSERT INTO ACCOUNTS (USERNAME,PASSWORD, NAME,ADMIN,STRONG, WEAK,  EMAIL) "  \
         "VALUES ('paul','paul', 'Paul', 0,'tennis','chess', 'dariae9@yahoo.com'); " \
         "INSERT INTO ACCOUNTS (USERNAME,PASSWORD, NAME, ADMIN, STRONG, WEAK,  EMAIL) "  \
         "VALUES ('andreea','andreea', 'Andreea', 1,'football','tennis',  'dariae9@yahoo.com'); " \
         "INSERT INTO ACCOUNTS (USERNAME,PASSWORD, NAME, ADMIN, STRONG, WEAK,  EMAIL) "  \
         "VALUES ('stefan','stefan', 'Stefan', 0,'chess', 'football',  'dariae9@yahoo.com'); " \
         "INSERT INTO ACCOUNTS (USERNAME, PASSWORD, NAME,ADMIN,STRONG, WEAK,  EMAIL) "  \
         "VALUES ('mihai', 'mihai', 'Mihai', 1,'tennis','chess' , 'dariae9@yahoo.com'); " \
         "INSERT INTO CHAMPIONSHIPS (NAME, TYPE, NB_PLAYERS, NR_PARTICIPANTI, PARTICIPANTI, STRUCTURE, HISTORY, WINNER, GAMES, ORA, LOCATION, DESC) "  \
         "VALUES ('International Chess', 'chess', 2, 1,'mihai', 'one round','12/4/15, 17/7/20','Andrei', '2/2/27','12:00','Romania, Bucharest','Chess is an abstract strategy game and involves no hidden information. This championship requires experience'); " \
         "INSERT INTO CHAMPIONSHIPS (NAME, TYPE, NB_PLAYERS, NR_PARTICIPANTI, PARTICIPANTI, STRUCTURE,HISTORY, WINNER, GAMES, ORA,LOCATION, DESC) "  \
         "VALUES ('National Chess', 'chess', 4,1,'andrei', 'three rounds', '12/4/17, 7/3/20','Monica', '4/4/23','10:00','Romania, Iasi','Chess is an abstract strategy game and involves no hidden information. This championship requires experience'); " \
         "INSERT INTO CHAMPIONSHIPS (NAME, TYPE, NB_PLAYERS, NR_PARTICIPANTI, PARTICIPANTI, STRUCTURE, HISTORY, WINNER, GAMES, ORA, LOCATION, DESC) "  \
         "VALUES ('Tennis for teams', 'tennis', 4, 0,'', '1 round','12/12/19','Team Australia', '23/5/24', '13:00','Romania, Cluj','Tennis is a racket sport that can be played individually against a single opponent: singles '); " \
         "INSERT INTO CHAMPIONSHIPS (NAME, TYPE, NB_PLAYERS, NR_PARTICIPANTI, PARTICIPANTI, STRUCTURE, HISTORY, WINNER, GAMES, ORA, LOCATION, DESC) "  \
         "VALUES ('Tennis', 'tennis', 2, 1,'andreea', '1 round','none','none', '15/3/23','16:00','UK, London','Tennis is a racket sport that can be played between two teams of two players each : doubles '); " \
         "INSERT INTO CHAMPIONSHIPS (NAME, TYPE, NB_PLAYERS, NR_PARTICIPANTI, PARTICIPANTI, STRUCTURE, HISTORY, WINNER, GAMES, ORA, LOCATION, DESC) "  \
         "VALUES ('Football', 'football', 2,1,'ana', '5 rounds', '12/12/13' , 'Team Denmark', '12/12/25','20:00','Germany, Berlin','Football is a game involving two teams of 11 players who try to maneuver the ball into goal without using their hands or arms ' ); " ;
         

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      printf("SQL error: %s\n", zErrMsg);
   } else {
      printf("Records created successfully\n");
   }
   sqlite3_close(db);
   return 0;
}