#include"bplustree.h"
#include"database.h"
#include<iostream>
#inclide<sstream>

void big_data_test(DBHANDLE* db, int num){
	// test to check correctness of operation with large volum of data and operations
	int wrong = 0;
	stringstream ss;
	string key;
	string data;
	// step 1: store num record
	for(int i = 1; i < num + 1; i++){
		ss << i;
		ss >> key;
		ss.clear();
		ss << 2 * i + 1;
		ss >> data;
		ss.clear();
		int flag = db_store(db, key, data, STORE);
		if (flag == -1){
			error++;
			cout << "a key inserting failed!" << endl;
		}
		if (i % 10000 == 0) cout << i << " records inserted!" << endl;
	}
	// step 2: fetch all num record and check its correctness
	for(int i = 1; i < num + 1; i++){
		ss << i
		ss >> key;
		ss.clear();
		ss << 2 * i + 1;
		ss >> data;
		string res = db_fetch(db, key);
		if(res != data){
			cout << "fetch wrong result:";
			cout << " should be " << data << " but got " << res <<endl;
			wrong++;
		}
	}
	// step 3: replace all record with different value
	for(int i = 1; i < num + 1; i++){
		ss << i
		ss >> key;
		ss.clear();
		ss << 2 * i + 2;
		ss >> data;
		db_fetch(db, key, data, REPLACE);
	}
	// step 4: fetch all num record and check its correctness
	for(int i = 1; i < num + 1; i++){
		ss << i
		ss >> key;
		ss.clear();
		ss << 2 * i + 2;
		ss >> data;
		ss.clear();
		string res = db_fetch(db, key);
		if(res != data){
			cout << "fetch wrong result:";
			cout << " should be " << data << " but got " << res <<endl;
			wrong++;
		}
	}
	// step 5: deleted num / 5 record randomly
	for(int i = 1; i < num / 5 + 1; i++){
		int del_key = rand()%num;
		ss << del_key;
		ss >> key;
		ss.clear();
		db_delete(db, key);
	}
	// step 6: insert num / 5 record randomly
	for(int i = 1; i < num / 5 + 1; i++){
		ss << i;
		ss >> key;
		ss.clear();
		ss << 2 * i + 2;
		ss >> data;
		ss.clear();
		int flag = db_store(db, key, data, STORE);
		if (flag == -1){
			error++;
			cout << "a key inserting failed!" << endl;
		}
	}
	// step 7: fetch num / 2 record randomly and check correctness
	for(int i = 1; i < num / 2 + 1; i++){
		ss << i
		ss >> key;
		ss.clear();
		ss << 2 * i + 2;
		ss >> data;
		ss.clear();
		string res = db_fetch(db, key);
		if(res != data){
			cout << "fetch wrong result:";
			cout << " should be " << data << " but got " << res <<endl;
			wrong++;
		}
	}
	if(wrong == 0) cout << "Big Data Test for correctness passed!" << endl;
	else cout << "Warning: Big Data Test for correctness failed" << endl;		
}

void complicated_corr_test(DBHANDLE* db, int num){
	// simulating real operation from user and check correctness
	int wrong = 0;
	int keyInt;
	int dataInt;
	string key;
	string data;
	stringstream ss;
	// step 1: insert num record into database
	for (int i = 0; i < num; i++){
		keyInt = 10 + i;
		dataInt = keyInt;
		ss << keyInt;
		ss >> key;
		ss.clear();
		data = key;
		db_store(db, key, data, STORE);
	}
	// step 2: fetch all num record and check correctness
	for (int i = 0; i < num; i++){
		keyInt = 10 + i;
		ss << keyInt;
		ss >> key;
		data = key;
		string res = db_fetch(db, key);
		if(res != data){
			cout << "fetch wrong result:";
			cout << " should be " << data << " but got " << res <<endl;
			wrong++;
		}
	}
	// setp 3: loop test with num * 5 loops
	int loops = 0;
	for (int i = 0; i < 5 * num; i++){
		keyInt = rand() % (10 + num);
		ss << keyInt;
		ss >> key;
		data = key;
		// step a: fetch one record randomly and check correctness
		string res = db_fetch(db, key);
		if(res != data){
			cout << "fetch wrong result:";
			cout << " should be " << data << " but got " << res <<endl;
			wrong++;
		}
		// step b: every 37 loops, delete a record randomly
		if (loops % 37 == 0){
			keyInt = rand() % (10 + num);
			ss << keyInt;
			ss >> key;
			db_delete(db, key);
		}
		// step c: every 11 loops, store a record randomly and fetch it,
		// then check the correctness
		if (loops % 11 == 0){
			keyInt = rand() % (10 + num);
			ss << keyInt;
			ss >> key;
			ss << keyInt;
			ss >> data;
			db_store(db, key, data, STORE);
			string res = db_fetch(db, key);
			if (res != data){
				cout << "fetch wrong result:";
				cout << " should be " << data << " but got " << res <<endl;
				wrong++;
			}
		}
		// step d: every 17 loops, replace a record with new value
		if (loops % 17 == 0){
			keyInt = rand() % (10 + num);
			ss << keyInt;
			ss >> key;
			ss << keyInt;
			ss >> data;
			db_store(db, key, data, REPLACE);
		}
	}
	if(wrong == 0) cout << "Complicated Operation Test for correctness passed!" << endl;
	else cout << "Warning: Complicated Operation Test for correctness failed" << endl;		
}


int main(){
	// enterance of test for correctness
	DBHANDLE* DB_1 = db_open("corr_test_1", CREATE);
	big_data_test(DB_1, 10000);
	db_close(DB_1);
	DBHANDLE* DB_2 = db_open("corr_test_2", CREATE);
	complicated_corr_test(DB_2, 100);
	db_close(DB_2);
	system("pause");
}














