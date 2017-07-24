#include"BPlusTree.h"
#include<string>
#include<fstream>
#include<cstring>
#include<sstream>

using namespace std;

enum STORE_TYPE{INSERT, REPLACE, STORE};
enum OPEN_TYPE{READ, WRITE, CREATE};

const char SPERATOT = ';';
const char SPACE = ' ';

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
	// void write_dat(const int offset, string data);
};

DBHANDLE::DBHANDLE(char *pathname, OPEN_TYPE oflag) {
	cout << pathname << endl;
	open_type = oflag;
	filename = pathname;
	string idx = pathname;
	idx += ".idx";
	string dat = pathname;
	dat += ".dat";
	cout << idx << " " << dat << endl;
	index_file.open(idx, ios::out);
	data_file.open(dat, ios::out);
	if (index_file) cout << "open!" << endl;
	tree = new Tree();
	filledMry = NULL;
	emptyMry = NULL;
	hook = NULL;
	if (oflag != CREATE) {
		char input;
		string key = "";
		string len = "";
		string offset = "";
		int spaceNum = 0;
		while (input = index_file.get()) {
			if (input == SPACE) {
				spaceNum++;
				continue;
			}
			else if (input == SPERATOT) {
				tree->insert(key, indexing(atoi(offset.c_str()), atoi(len.c_str())));
				mrySpace* new_space = new mrySpace(atoi(offset.c_str()), atoi(len.c_str()));
				filledMry->preSpace = new_space;
				new_space->nextSpace = filledMry;
				filledMry = new_space;
				key = "";
				len = "";
				offset = "";
				spaceNum = 0;
				continue;
			}
			else {
				if (spaceNum == 0) key += input;
				if (spaceNum == 1) offset += input;
				if (spaceNum == 2) len += input;
			}
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
			index_file << header->keys[i];
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
					maxOffSet = filledMry->offset + filledMry->len + 1;
				}
				else maxOffSet = 0;
				mrySpace* new_space = new mrySpace(maxOffSet, dataLen);
				new_space->nextSpace = filledMry;
				if(filledMry) filledMry->preSpace = new_space;
				filledMry = new_space;
				int len = data.size();
				tree->insert(key, indexing(maxOffSet, len));
				data_file.seekg(maxOffSet);
				//cout << data << endl;
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
				data_file.seekg(offset);
				//cout << data << endl;
				data_file << data;
			}
			else{
				// former memory space is not enough for new data
				int maxOffSet = filledMry->offset + filledMry->len + 1;
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
				}
				dataNode->data[target.second].len = dataLen;
				dataNode->data[target.second].offset = maxOffSet;
				mrySpace* new_space = new mrySpace(maxOffSet, dataLen);
				new_space->nextSpace = filledMry;
				if(filledMry) filledMry->preSpace = new_space;
				filledMry = new_space;
				data_file.seekg(maxOffSet);
				//cout << data << endl;
				data_file << data;
			}
		}
		return 0;
	}
}

string DBHANDLE::Fetch(string key){
	pair<Node* , int> target = tree->search(key);
	if(!target.first) return NULL;
	Node* dataNode = target.first;
	int index = target.second;
	int len = dataNode->data[index].len;
	int offset = dataNode->data[index].offset;
	data_file.seekg(offset);
	string output;
	for (int i = 0; i < len; i++) output += data_file.get();
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
				if(!emptyMry) emptyMry->preSpace = header;
				emptyMry = header;
			}
		}
		tree->remove(key);
	}
}

DBHANDLE* db_open(char *pathname, OPEN_TYPE oflag){
	string idxfile = pathname;
	idxfile += ".idx";
	string datfile = pathname;
	datfile += ".dat";
	fstream input_index;
	fstream input_data;
	input_index.open(idxfile, ios::in);
	input_data.open(datfile, ios::in);
	if(oflag != CREATE){
		if(input_index&&input_data){
		// the index file and data has existed
		return new DBHANDLE(pathname, oflag);
		}
		else cerr << "can't find and open index file and data file.";
	}
	if(oflag == CREATE){
		return	new DBHANDLE(pathname, oflag);
	}
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
	DBHANDLE* DB = db_open("demo", CREATE);
	for (int i = 0; i < 1000000; i++) {
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
		//cout << i << endl;
	}
	DB->write_idx();
	db_close(DB);
	system("pause");
}