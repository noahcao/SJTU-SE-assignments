# DataBase
A repo to store different simple database using different data structure.

Parts have been done:

1. A simple database system based on B plus tree data structure. For which, two code files are provided:

   * **bplustree.h**: file to provide the fundamental datastructure of the system. Note that, related theorical knowledge could be found in a few learning material;
   * **database.h**: file to provide the main body of database, which will be built and operated according to two distinct file, *XXX.idx and XXX.dat*, among which two, the former is designed to hold the relationshil between key and value ,and the latter designed to store the data information;

   The way to apply the interfaces of database has been shown in several test files, which are **demotest.cpp, spacetest.cpp, corrtest.cpp and timetest.cpp**.
