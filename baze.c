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
      "ID CHAR(4) PRIMARY KEY NOT NULL);";

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

   sql = "INSERT INTO ACCOUNTS (USERNAME, NAME,ADMIN, ID) "  \
         "VALUES ('daria', 'Daria', 1, 1); " \
         "INSERT INTO ACCOUNTS (USERNAME, NAME,ADMIN, ID) "  \
         "VALUES ('paul', 'Paul', 0, 2); " \
         "INSERT INTO ACCOUNTS (USERNAME, NAME, ADMIN, ID) "  \
         "VALUES ('andreea', 'Andreea', 1, 3); " \
         "INSERT INTO ACCOUNTS (USERNAME, NAME,ADMIN, ID) "  \
         "VALUES ('stefan', 'Stefan', 0, 4); " \
         "INSERT INTO CHAMPIONSHIPS (NAME,TYPE,NB_PLAYERS,STRUCTURE) "  \
         "VALUES ('International Chess', 'sah', 2, 'one round'); " \
        "INSERT INTO CHAMPIONSHIPS (NAME,TYPE,NB_PLAYERS,STRUCTURE) "  \
         "VALUES ('National Chess', 'sah', 4, 'three rounds'); " ;

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      printf("SQL error: %s\n", zErrMsg);
   } else {
      printf("Records created successfully\n");
   }
   // sql = "SELECT COUNT(*) FROM ACCOUNTS WHERE USERNAME='daria'";
   // rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   // if( rc != SQLITE_OK ){
   //    printf("SQL error: %s\n", zErrMsg);
   // } else {
   //    printf("Records created successfully\n");
   // }
   sqlite3_close(db);
   return 0;
}