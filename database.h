#include"BPlusTree.h"
#include<string>
#include<fstream>
#include<cstring>
#include<sstream>

using namespace std;

const int cacheSize = 10;

enum STORE_TYPE { INSERT, REPLACE, STORE };
enum OPEN_TYPE { READ, WRITE, CREATE };

const char SPERATOT = ';';
const char SPACE = ' ';

bool memFound(vector<pair<string, string>> cache, string key) {
	if (cache.empty()) return false;
	pair<string, string> header;
	for (int i = 0; i < cache.size(); i++){
		if (cache[i].first == key) return true;
	}
	return false;
}

string getMem(vector<pair<string, string>> cache, string key) {
	for (int i = 0; i < cache.size(); i++) {
		if (cache[i].first == key) return cache[i].second;
	}
}

void addCache(vector<pair<string, string>> cache, string key, string data) {
	if (cache.size() == cacheSize) {
		cache.erase(cache.end() - 1);
		cache.insert(cache.begin(), pair<string, string>(key, data));
	}
	else cache.insert(cache.begin(), pair<string, string>(key, data));
}

class mrySpace{
public:
	mrySpace(int offSet, int Len){
		len = Len;
		offset = offSet;
		preSpace = NULL;
		nextSpace = NULL;
	}
	int len;
	int offset;
	mrySpace* preSpace;
	mrySpace* nextSpace;
};

class DBHANDLE{
public:
	DBHANDLE(char *pathname, OPEN_TYPE oflag);
	Tree* tree;
	Node* header;
	Node* hook;
	mrySpace *filledMry;
	mrySpace *emptyMry;
	string filename;
	OPEN_TYPE open_type;
	fstream index_file;
	fstream data_file;
	int Store(string key, string data, STORE_TYPE flag);
	string Fetch(string key);
	int Delete(string key);
	void rewind();
	// char *nextrec(string key);
	bool reUse(string key, string data);
	void write_idx(); // edit idx file when close the db
	vector<pair<string, string>> cache;
};

DBHANDLE::DBHANDLE(char *pathname, OPEN_TYPE oflag) {
	open_type = oflag;
	filename = pathname;
	string idx = pathname;
	idx += ".idx";
	string dat = pathname;
	dat += ".dat";
	if (oflag == CREATE) {
		index_file.open(idx, ios::out);
		data_file.open(dat, ios::out);
	}
	if (oflag == READ) {
		index_file.open(idx, ios::in);
		data_file.open(dat, ios::in);
	}
	if (oflag == WRITE) {
		index_file.open(idx, ios::out|ios::in);
		data_file.open(dat, ios::out|ios::in);
	}
	tree = new Tree();
	filledMry = NULL;
	emptyMry = NULL;
	hook = NULL;
	index_file.seekg(0);
	data_file.seekg(0);
	if (oflag != CREATE) {
		string key;
		string len;
		string offset;
		//while (input = index_file.get()) {
		while(!index_file.eof()){
			index_file >> key;
			if (key.size() == 0) break;
			index_file >> offset;
			index_file >> len;
			len.erase(len.end() - 1);
			tree->insert(key, indexing(atoi(offset.c_str()), atoi(len.c_str())));
			mrySpace* new_space = new mrySpace(atoi(offset.c_str()), atoi(len.c_str()));
			if(filledMry != NULL) filledMry->preSpace = new_space;
			new_space->nextSpace = filledMry;
			filledMry = new_space;
			key = "";
			len = "";
			offset = "";
		}
		header = tree->leafHeader;
		hook = tree->leafHeader;
	}
}

void DBHANDLE::write_idx(){
	index_file.clear();
	Node* header = tree->leafHeader;
	while(header != NULL){
		for(int i = 0; i < header->keys.size(); i++){
			// cout << i << SPACE;
			index_file << SPACE << header->keys[i];
			// cout << header->keys[i];
			index_file << SPACE;
			// cout << SPACE;
			index_file << header->data[i].offset;
			// cout << header->data[i].offset;
			index_file << SPACE;
			// cout << SPACE;
			index_file << header->data[i].len;
			// cout << header->data[i].len;
			index_file << SPERATOT;
			// cout << SPERATOT << endl;
		}
		header = header->nextLeaf;
	}
	return;
}

void DBHANDLE::rewind(){
	hook = tree->leafHeader;
}

/*
char *DBHANDLE::nextrec(string key){
	pair<Node*, int> target = tree->search(key);
	Node* dataNode = target.first;
	int index = target.second;
	if(dataNode == NULL) return NULL;
	else{
		if(dataNode->nextLeaf == NULL){
			if(index == dataNode->keys.size() - 1) return NULL;
			else{
				return dataNode->data[index + 1];
			}
		}
	}

}*/

bool DBHANDLE::reUse(string key, string data){
	int needLen = data.size();
	mrySpace* header = emptyMry;
	while(header != NULL){
		if(header->len >= needLen){
			int pos = header->offset;
			tree->insert(key, indexing(header->offset, needLen));
			// pick out the memory space from the empty space link
			if(header->preSpace) header->preSpace->nextSpace = header->nextSpace;
			if(header->nextSpace) header->nextSpace->preSpace = header->preSpace;
			if((!header->preSpace)&&(!header->nextSpace)) emptyMry = NULL;
			// add the memory space into the filled space link
			if(filledMry == NULL){
				// case when no memory space is being used
				filledMry = header;
				header->preSpace = NULL;
				header->nextSpace = NULL;
			}
			else{
				if(header->offset >= filledMry->offset){
					header->nextSpace = filledMry;
					filledMry->preSpace = header;
					header->preSpace = NULL;
					filledMry = header;
				}
				else{
					header->preSpace = filledMry;
					header->nextSpace = filledMry->nextSpace;
					if(filledMry->nextSpace) filledMry->nextSpace->preSpace = header;
					filledMry->nextSpace = header;
				}	
			}
			tree->insert(key, indexing(pos, needLen));
			data_file.seekg(pos);
			data_file << data;
			return true;
		}
	}
	return false;
}

int DBHANDLE::Store(string key, string data, STORE_TYPE flag){
	if (memFound(cache, key) && (flag == INSERT)) return -1;
	pair<Node*, int> target = tree->search(key);
	bool existed = target.first != NULL;
	if((existed)&&(flag == INSERT)) return 1;
	if((!existed)&&(flag == REPLACE)) return -1;
	else{
		int dataLen = data.size();
		if(!existed){
			bool reuse = reUse(key, data);
			// firstly, try to reuse memory released before
			if(!reuse){
				int maxOffSet;
				if (filledMry) {
					maxOffSet = filledMry->offset + filledMry->len;
				}
				else maxOffSet = 0;
				mrySpace* new_space = new mrySpace(maxOffSet, dataLen);
				new_space->nextSpace = filledMry;
				if(filledMry) filledMry->preSpace = new_space;
				filledMry = new_space;
				int len = data.size();
				tree->insert(key, indexing(maxOffSet, len));
				data_file.seekp(maxOffSet);
				data_file << data;
			}
		}
		else{
			Node* dataNode = target.first;
			int index = target.second;
			if(dataNode->data[target.second].len >= dataLen){
				// memory enough for overwrite
				dataNode->data[target.second].len = dataLen;
				int offset = dataNode->data[target.second].offset;
				mrySpace *mryBlock = filledMry;
				while (mryBlock != NULL) {
					if (mryBlock->offset == offset) {
						mryBlock->len = dataLen;
						break;
					}
					else mryBlock = mryBlock->nextSpace;
				}
				data_file.seekp(offset);
				data_file << data;
			}
			else{
				// former memory space is not enough for new data
				int maxOffSet = filledMry->offset + filledMry->len;
				mrySpace *mryBlock = filledMry;
				int offset = dataNode->data[index].offset;
				while(mryBlock != NULL){
					// throw former memory space into the empty space link
					if(mryBlock->offset == offset){
						if(mryBlock->preSpace) mryBlock->preSpace->nextSpace = mryBlock->nextSpace;
						if(mryBlock->nextSpace) mryBlock->nextSpace->preSpace = mryBlock->preSpace;
						if((!mryBlock->preSpace)&&(!mryBlock->nextSpace)) filledMry = NULL;
						mryBlock->preSpace = NULL;
						mryBlock->nextSpace = emptyMry;
						emptyMry->preSpace = mryBlock;
						emptyMry = mryBlock;
						break;
					}
					mryBlock = mryBlock->nextSpace;
				}
				dataNode->data[target.second].len = dataLen;
				dataNode->data[target.second].offset = maxOffSet;
				mrySpace* new_space = new mrySpace(maxOffSet, dataLen);
				new_space->nextSpace = filledMry;
				if(filledMry) filledMry->preSpace = new_space;
				filledMry = new_space;
				data_file.seekp(maxOffSet);
				data_file << data;
			}
		}
		return 0;
	}
}

string DBHANDLE::Fetch(string key){
	// firstly, check the cache for searching key
	if (memFound(cache, key)) return getMem(cache, key);
	pair<Node* , int> target = tree->search(key);
	if(!target.first) return NULL;
	Node* dataNode = target.first;
	int index = target.second;
	int len = dataNode->data[index].len;
	int offset = dataNode->data[index].offset;
	data_file.seekg(offset);
	string output;
	for (int i = 0; i < len; i++) output += data_file.get();
	addCache(cache, key, output);
	return output;
}

int DBHANDLE::Delete(string key){
	pair<Node*, int> target = tree->search(key);
	if(!target.first) return -1;
	else{
		int index = target.second;
		Node* dataNode = target.first;
		int offset = dataNode->data[index].offset;
		int len = dataNode->data[index].len;
		mrySpace *header = filledMry;
		while(header != NULL){
			if(header->offset == offset){
				if(header->preSpace) header->preSpace->nextSpace = header->nextSpace;
				else if(header->nextSpace) header->nextSpace->preSpace = header->preSpace;
				else filledMry = NULL;
				header->preSpace = NULL;
				header->nextSpace = emptyMry;
				if(emptyMry) emptyMry->preSpace = header;
				emptyMry = header;
				break;
			}
			header = header->nextSpace;
		}
		tree->remove(key);
		return 0;
	}
}

DBHANDLE* db_open(char *pathname, OPEN_TYPE oflag){
	string idxfile = pathname;
	idxfile += ".idx";
	string datfile = pathname;
	datfile += ".dat";
	fstream input_index;
	fstream input_data;
	if (oflag == READ) {
		input_index.open(idxfile, ios::in);
		input_data.open(datfile, ios::in);
		if (!(input_index&&input_data)) {
			cerr << "can't find and open index file and data file.";
		}
	}
	return new DBHANDLE(pathname, oflag);
}

void db_close(DBHANDLE* db){
	db->index_file.close();
	db->data_file.close();
}


int db_store(DBHANDLE* db, string key, string data, STORE_TYPE flag){
	return db->Store(key, data, flag);
}


string db_fetch(DBHANDLE* db, string key){
	return db->Fetch(key);
}


int db_delete(DBHANDLE* db, string key){
	return db->Delete(key);
}

void db_rewind(DBHANDLE* db){
	db->rewind();
	return;
}

/*
char *db_nextrec(DBHANDLE db, string key){
	return db.nextrec(key);
}*/

int main() {
	DBHANDLE* DB = db_open("demo",WRITE);
	for (int i = 0; i < 10000000; i++) {
		stringstream ss;
		ss << i;
		string key;
		string data;
		ss >> key;
		ss.clear();
		ss << 2 * i + 1;
		ss >> data;
		db_store(DB, key, data, INSERT);
		if (i % 10000 == 0) cout << i << endl;
	}
	int flag;
	flag = db_delete(DB, "3");
	cout << flag << endl;
	flag = db_delete(DB, "200");
	cout << flag << endl;
	flag = db_delete(DB, "322");
	cout << flag << endl;
	flag = db_delete(DB, "120");
	cout << flag << endl;
	 DB->write_idx();
	string out = db_fetch(DB, "787");
	cout << out << endl;
	out = db_fetch(DB, "87");
	cout << out << endl;
	out = db_fetch(DB, "117");
	cout << out << endl;
	out = db_fetch(DB, "77");
	cout << out<< endl;
	out = db_fetch(DB, "7");
	cout << out << endl;
	db_store(DB, "1", "1212121", REPLACE);
	cout << "replace finish" << endl;
	out = db_fetch(DB, "1");
	cout << out << endl;
	out = db_fetch(DB, "787");
	cout << out << endl;
	out = db_fetch(DB, "87");
	cout << out << endl;
	out = db_fetch(DB, "117");
	cout << out << endl;
	out = db_fetch(DB, "77");
	cout << out << endl;
	out = db_fetch(DB, "7");
	cout << out << endl;
	DB->write_idx();
	db_close(DB);
	system("pause");
}