// file to test the database
#include"database.h"

void insertNRandom(int num, DBHANDLE* db) {
	stringstream ss;
	for(int i = 1; i < num + 1; i++){
		string key;
		string data;
		ss << i;
		ss >> key;
		ss << 2 * i + 1;
		ss.clear();
		ss >> data;
		ss.clear();
		int flag = db_store(db, key, data, STORE);
		if (flag == -1){
			cout << "a key inserting failed!" << endl;
		}
		if (i % 10000 == 0) cout << i << " keys inserted!" << endl;
	}
	cout << "Has successfully insert " << num << "keys into the database!" << endl;
}

void deleteNRandom(int num, DBHANDLE* db){
	stringstream ss;
	for(int i = 1; i < num + 1; i++){
		string key;
		int delKey = rand()%10000;
		ss << delKey;
		ss >> key;
		ss.clear();
		db_delete(db, key);
		if (i % 100 == 0) cout << i << " keys deleted" << endl;
	}
}

void fetchNRandom(int num, DBHANDLE* db){
	stringstream ss;
	for(int i = 0; i < num; i++){
		string key;
		int fetchKey = rand()%10000;
		ss << fetchKey;
		ss >> key;
		ss.clear();
		string output = db_fetch(db, key);
		if(output.size() == 0) continue;
		else{
			int trueRes = 2 * fetchKey + 1;
			if(atoi(output.c_str()) != trueRes) cout << "fetch wrong result!" << endl;
		}
	}
}

void replaceNRandom(int num, DBHANDLE* db){
	stringstream ss;
	for(int i = 0; i < num; i++){
		string key;
		string data;
		int replaceKey = rand()%10000;
		int replaceData = replaceKey * 2 + 2;
		ss << replaceKey;
		ss >> key;
		ss.clear();
		ss << replaceData;
		ss >> data;
		ss.clear();
		int flag = db_store(db, key, data, REPLACE);
	}
}


int main() {
	DBHANDLE* DB = db_open("demo", WRITE);
	insertNRandom(10000, DB);
	deleteNRandom(800, DB);
	fetchNRandom(1000, DB);
	replaceNRandom(2000, DB);
	db_close(DB);
	system("pause");
}