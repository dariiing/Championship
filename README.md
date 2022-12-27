# Championship Project

*About:* Sa se realizeze o aplicatie client/server ce va administra diferite campionate. Aplicatia server va avea urmatoarele functionalitati: inregistrarea utilizatorilor(diferite tipuri de utilizatori: obisnuiti, administratori). Toate comenzile vor fi restrictionate de sectiunea de logare. Dupa logare utilizatorii vor putea primi informatii despre ultimele campionate inregistrate, avand posibilitatea de a se inscrie la ele. Comenzile ce trebuie implementate: inregistrarea unui campionat, specificarea jocului, numarului de jucatori, diferite reguli sau structuri de campionat(single-elimination, double elimination), modul de extragere a partidelor(deciderea partidelor), inregistrarea unui utiliziator intr-un campionat, utilizatorul va fi informat via e-mail daca a fost acceptat in campionatul respectiv, si va primi informatii aditionale despre partidele sale (ora, adversarul, etc...) Utilizatorul are posibilitatea sa reprogrameze o sesiune de joc. Utilizatorii administratori vor detine un istoric al scorurilor partidelor.

Comenzi:
 gcc baze.c -o baze -lsqlite3
 ./baze
 make
 ./client