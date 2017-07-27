// file to test the database
#include"database.h"
#include"time.h"

void insertNRandom(int num, DBHANDLE* db) {
	int error = 0;
	stringstream ss;
	for(int i = 1; i < num + 1; i++){
		string key;
		string data;
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
	cout << "insert test finished: passed " << num - error << "/" << num << " in total." << endl;
}

void deleteNRandom(int num, DBHANDLE* db){
	stringstream ss;
	for(int i = 1; i < num + 1; i++){
		string key;
		int delKey = rand()%1000000;
		ss << delKey;
		ss >> key;
		ss.clear();
		db_delete(db, key);
	}
	cout << "randomly delete test finished" << endl;
}

void fetchNRandom(int num, DBHANDLE* db){
	int error = 0;
	stringstream ss;
	for(int i = 0; i < num; i++){
		string key;
		int fetchKey = rand()%10000;
		ss << fetchKey;
		ss >> key;
		ss.clear();
		string output = db_fetch(db, key);
		if (output.size() == 0){
			continue;
		}
		else{
			int trueRes = 2 * fetchKey + 1;
			if (atoi(output.c_str()) != trueRes){
				cout << "error occurs!" << " True outpout should be " << trueRes << " but get " << output << endl;
				error++;
			}
		}
	}
	cout << "fetch test finished: passed " << num - error << "/" << num << " in total." << endl;
}

void replaceNRandom(int num, DBHANDLE* db){
	stringstream ss;
	for(int i = 0; i < num; i++){
		string key;
		string data;
		int replaceKey = rand()%10000;
		int replaceData = replaceKey * 2 + 1;
		ss << replaceKey;
		ss >> key;
		ss.clear();
		ss << replaceData;
		ss >> data;
		ss.clear();
		int flag = db_store(db, key, data, REPLACE);
	}
	cout << "randomly replace test finished" << endl;
}

void loopTest(int num, DBHANDLE* db){
	int error = 0;
	for (int i = 0; i < num; i++){
		int keyInt = 1000010 + i;
		int dataInt = keyInt;
		string key;
		string data;
		stringstream ss;
		ss << keyInt;
		ss >> key;
		ss.clear();
		ss << dataInt;
		ss >> data;
		db_store(db, key, data, STORE);
		string firstRes = db_fetch(db, key);
		db_delete(db, key);
		string secondRes = db_fetch(db, key);
		int thirdflag = db_store(db, key, data, REPLACE);
		int aa = db_store(db, key, data, STORE);
		string forthRes = db_fetch(db, key);
		if ((firstRes != data) || (secondRes != "") || (thirdflag != -1) || (forthRes != data)){
			error++;
			cout << "failed when do loop test!" << endl;
		}
	}
	cout << "loop test finished: passed " << num - error << "/" << num << " in total." << endl;
}

void performanceTest(int num, DBHANDLE* db){
	int keyInt;
	int dataInt;
	string key;
	string data;
	stringstream ss;
	double start = clock();
	for (int i = 0; i < num; i++){
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
		keyInt = 10 + i;
		ss << keyInt;
		ss >> key;
		string res = db_fetch(db, key);
	}
	int loops = 0;
	for (int i = 0; i < 5 * num; i++){
		keyInt = rand() % (10 + num);
		ss << keyInt;
		ss >> key;
		data = db_fetch(db, key);
		if (loops % 37 == 0){
			keyInt = rand() % (10 + num);
			ss << keyInt;
			ss >> key;
			db_delete(db, key);
		}
		if (loops % 11 == 0){
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
	cout << "performance test finished!" << endl;
	cout << "with nrec = " << num << " time used is " << duration << "s" << endl;
}

int main() {
	// test the database through several main operations
	DBHANDLE* DB = db_open("demo", CREATE);
	insertNRandom(1000000, DB);
	deleteNRandom(500000, DB);
	insertNRandom(1000000, DB);
	//fetchNRandom(10000, DB);
	//replaceNRandom(2000, DB);
	//fetchNRandom(1000, DB);
	//loopTest(1000, DB);
	//performanceTest(10000, DB);
	db_close(DB);
	system("pause");
}