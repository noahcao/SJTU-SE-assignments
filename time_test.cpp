#include"database.h"
#include<sstream>
#include<iostream>
#include"time.h"

void complicated_time_test(DBHANDLE* db, int num){
	int keyInt;
	int dataInt;
	string key;
	string data;
	stringstream ss;
	double start = clock();
	for (int i = 0; i < num; i++){
		// step 1: insert num record into database
		keyInt = 10 + i;
		dataInt = keyInt;
		ss << keyInt;
		ss >> key;
		ss.clear();
		ss << dataInt;
		ss >> data;
		ss.clear();
		db_store(db, key, data, STORE);
	}
	for (int i = 0; i < num; i++){
		// step 2: fetch num record from database
		keyInt = 10 + i;
		ss << keyInt;
		ss >> key;
		string res = db_fetch(db, key);
	}
	int loops = 0;
	// step 3: loop test with loop number num * 5
	for (int i = 0; i < 5 * num; i++){
		keyInt = rand() % (10 + num);
		ss << keyInt;
		ss >> key;
		// step a: fetch a record randomly
		data = db_fetch(db, key);
		if (loops % 37 == 0){
			// step b: every 37 loops, delete a record randomly
			keyInt = rand() % (10 + num);
			ss << keyInt;
			ss >> key;
			db_delete(db, key);
		}
		if (loops % 11 == 0){
			// step c: every 11 loops, store a record randomly
			// and fetch this record
			keyInt = rand() % (10 + num);
			ss << keyInt;
			ss >> key;
			ss << keyInt;
			ss >> data;
			db_store(db, key, data, STORE);
			string res = db_fetch(db, key);
			if (res != data) cout << "wrong fetch result£¡" << endl;
		}
		if (loops % 17 == 0){
			// step d: every 17 loops, replace a record with new value
			// in two following replacement operations, once replacing
			// with length-equal value, the other using a longer one
			keyInt = rand() % (10 + num);
			ss << keyInt;
			ss >> key;
			ss << keyInt;
			ss >> data;
			if (loops % 34 == 17) db_store(db, key, data, REPLACE);
			if (loops % 34 == 0){
				data = data + "1";
				db_store(db, key, data, REPLACE);
			}
		}
	}
	for (int i = 0; i < num; i++){
		// step 4: delete all recorded information, after deleteing
		// one record, randomly fetch 10 records
		keyInt = 10 + i;
		dataInt = keyInt;
		ss << keyInt;
		ss >> key;
		ss.clear();
		ss << dataInt;
		ss >> data;
		ss.clear();
		db_delete(db, key);
		for (int i = 0; i < 10; i++){
			keyInt = rand() % (10 + num);
			ss << keyInt;
			ss >> key;
			string res = db_fetch(db, key);
		}
	}
	double end = clock();
	double duration = ((double)(end - start)) / CLK_TCK;
	// print the consumed time
	cout << "performance test finished!" << endl;
	cout << "with nrec = " << num << " time used is " << duration << "s" << endl;
}

void volume_nodesize_test(DBHANDLE* db, int nrec){
	int keyInt;
	int dataInt;
	string key;
	string data;
	stringstream ss;
	double start = clock();
	// step 1: insert nrec records and return the consumed time
	for(int i = 0; i < nrec; i++){
		keyInt = i;
		dataInt = keyInt;
		ss << keyInt;
		ss >> key;
		ss << dataInt;
		ss >> data;
		db_store(db, key, data, STORE);
	}
	double end = clock();
	cout << "Time used to insert " << nrec << " record is ";
	cout << ((double)(end - start)) / CLK_TCK << " s" << endl;
	start = clock();
	// step 2: read nrec records and return the consumed time
	for(int i = 0; i < nrec; i++){
		keyInt = i;
		ss << keyInt;
		ss >> key;
		string res = db_fetch(db, key);
	}
	end = clock();
	cout << "Time used to read " << nrec << " record is ";
	cout << ((double)(end - start)) / CLK_TCK << " s" << endl;
	start = clock();
	// step 3: modify all nrec records and return the consumed time
	for(int i = 0; i < nrec; i++){
		keyInt = i;
		dataInt = keyInt;
		ss << keyInt;
		ss >> key;
		ss << dataInt;
		ss >> data;
		db_store(db, key, data, REPLACE);
	}
	end = clock();
	cout << "Time used to replace " << nrec << " record is ";
	cout << ((double)(end - start)) / CLK_TCK << " s" << endl;
	start = clock();
	// step 4: delete randomly nrec / 5 records and return the consumed time
	for(int i = 0; i < nrec / 5; i++){
		keyInt = rand()%(nrec - 1);
		ss << keyInt;
		ss >> key;
		db_delete(db, key);
	}
	end = clock();
	cout << "Time used to delete " << nrec / 5 << " record is ";
	cout << ((double)(end - start)) / CLK_TCK << " s" << endl;
}

void buffer_time_test(DBHANDLE* db){
	stringstream ss;
	string key;
	string data;
	double start = clock();
	for(int i = 0; i < 1000; i++){
		// step 1: insert 50 records randomly
		for(int k = 0; k < 50; k++){
			keyInt = rand()%1000;
			dataInt = keyInt;
			ss << keyInt;
			ss >> key;
			ss << dataInt;
			ss >> data;
			db_store(db, key, data, STORE);
		}
		// step 2: delte 20 records randomly
		for(int k = 0; k < 20; k++){
			keyInt = rand()%1000;
			ss << keyInt;
			ss >> key;
			ss << dataInt;
			ss >> data;
			db_store(db, key, data, REPLACE);
		}
		// step 3: fetch 20 records randomly
		for(int k = 0; k < 20; k++){
			keyInt = rand()%1000;
			ss << keyInt;
			ss >> key;
			string res = db_fetch(db, key);
		}
		// step 4: delete 20 records randomly
		for(int k = 0; k < 20; k++){
			keyInt = rand()%1000;
			ss << keyInt;
			ss >> key;
			db_delete(db, key);
		}
		// step 5: fetch 20 records randomly
		for(int k = 0; k < 20; k++){
			keyInt = rand()%1000;
			ss << keyInt;
			ss >> key;
			string res = db_fetch(db, key);
		}
	}
	double end = clock();
	cout << "Time used totally is ";
	cout << ((double)(end - start)) / CLK_TCK << " s" << endl;
}

int main(){
	// enterance to do test on time lantency
	DBHANDLE* DB_1 = db_open("time_test_1", CREATE);
	complicated_time_test(DB_1, 10000);
	db_close(DB_1);
	// DB_2 is both for the test about data volume and
	// about node size, you should change the nodesize
	// parameter in file "bplustree.h"
	DBHANDLE* DB_2 = db_open("time_test_2", CREATE);
	complicated_time_test(DB_2, 10000);
	db_close(DB_2);
	// DB_3 is for the test about buffer, you should change
	// the parameter bufferSize in file "database.h"
	DBHANDLE* DB_3 = db_open("time_test_3", CREATE);
	buffer_time_test(DB_3);
	db_close(DB_3);
	system("pause");
}














