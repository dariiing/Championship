#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("bau\n");
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
      "USERNAME CHAR(10) NOT NULL,"\
      "NAME TEXT NOT NULL,"\
      "ADMIN INT NOT NULL,"\
      "ID CHAR(4) PRIMARY KEY NOT NULL,"\
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
      "STRUCTURE TEXT NOT NULL,"\
      "HISTORY TEXT,"\
      "GAMES DATE);";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      printf("SQL error: %s\n", zErrMsg);
   } else {
      printf("Table created successfully\n");
   }

   sql = "INSERT INTO ACCOUNTS (USERNAME, NAME,ADMIN, ID, EMAIL) "  \
         "VALUES ('daria', 'Daria', 1, 1, 'dariae9@yahoo.com'); " \
         "INSERT INTO ACCOUNTS (USERNAME, NAME,ADMIN, ID, EMAIL) "  \
         "VALUES ('paul', 'Paul', 0, 2, 'dariae9@yahoo.com'); " \
         "INSERT INTO ACCOUNTS (USERNAME, NAME, ADMIN, ID, EMAIL) "  \
         "VALUES ('andreea', 'Andreea', 1, 3, 'dariae9@yahoo.com'); " \
         "INSERT INTO ACCOUNTS (USERNAME, NAME,ADMIN, ID, EMAIL) "  \
         "VALUES ('stefan', 'Stefan', 0, 4, 'dariae9@yahoo.com'); " \
         "INSERT INTO ACCOUNTS (USERNAME, NAME,ADMIN, ID, EMAIL) "  \
         "VALUES ('mihai', 'Mihai', 1, 5, 'dariae9@yahoo.com'); " \
         "INSERT INTO CHAMPIONSHIPS (NAME,TYPE,NB_PLAYERS,STRUCTURE,HISTORY,GAMES) "  \
         "VALUES ('International Chess', 'chess', 2, 'one round','12/4/15, 17/7/20', '2/2/27'); " \
        "INSERT INTO CHAMPIONSHIPS (NAME,TYPE,NB_PLAYERS,STRUCTURE,HISTORY,GAMES) "  \
         "VALUES ('National Chess', 'chess', 4, 'three rounds', '12/4/17, 7/3/20', '4/4/23'); "
         "INSERT INTO CHAMPIONSHIPS (NAME,TYPE,NB_PLAYERS,STRUCTURE,HISTORY,GAMES) "  \
         "VALUES ('Tennis for teams', 'tennis', 4, '1 round','12/12/19', '23/5/24'); "
         "INSERT INTO CHAMPIONSHIPS (NAME,TYPE,NB_PLAYERS,STRUCTURE,HISTORY,GAMES) "  \
         "VALUES ('Tennis', 'tennis', 2, '1 round','never played', '15/3/23'); "
         "INSERT INTO CHAMPIONSHIPS (NAME,TYPE,NB_PLAYERS,STRUCTURE,HISTORY,GAMES) "  \
         "VALUES ('Football', 'football', 2, '5 rounds', '12/12/13', '12/12/25'); " ;

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