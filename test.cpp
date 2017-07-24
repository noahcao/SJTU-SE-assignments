// file to test the database
#include"database.h"

void insertNRandom(int num, DBHANDLE* db) {

}

int main() {
	DBHANDLE* DB = db_open("demo", WRITE);
	insertNRandom(1000000, DB);
}