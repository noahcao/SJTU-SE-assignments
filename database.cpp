#include"BPlusTree.h"
#include<string>
#include<fstream>

using namespace std;

enum STORE_TYPE{INSERT, REPLACE, STORE};
enum OPEN_TYPE{READ, WRITE, CREATE};

const char SPERATOT = ';'；
const char SPACE = ' ';

class mrySpace{
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
}

class DBHANDLE{
public:
	DBHANDLE(char *pathname, OPEN_TYPE oflag){
		filename = pathname;
		Tree<string> tree = new Tree<string>();
		filledMry = NULL;
		emptyMry = NULL;
	}
	DBHANDLE(char *pathname, fstream input_index, fstream input_data, OPEN_TYPE oflag){
		filename = pathname;
		Tree<string> tree = new Tree<string>();
		filledMry = NULL;
		emptyMry = NULL;
		char input;
		string key = "";
		string len = "";
		string offset = "";
		int spaceNum = 0;
		while(input = input_index.get()){
			if(input == SPACE){
				spaceNum++;
				continue;
			}
			else if(input == SPERATOT){
				tree.insert(key, offset + " " + len);
				mrySpace new_space = new mrySpace(a_toi(offset.c_str()), atoi(len.c_str());
				filledMry->preSpace = new_space;
				new_space->nextSpace = filledMry;
				filledMry = new_space;
				key = "";
				len = "";
				offset = "";
				spaceNum = 0;
				continue;
			}
			else{
				if(spaceNum == 0) key += input;
				if(spaceNum == 1) offset += input;
				if(spaceNum == 2) len += input;
			}
		}
	}
	Tree<string>* tree;
	Node<string>* header;
	Node<string>* hook;
	mrySpace *filledMry;
	mrySpace *emptyMry;
	string filename;
	OPEN_TYPE open_type;
	fstream index_file;
	fstream data_file;
	int Store(const char *key, const char *data, STORE_TYPE flag);
	char *Fetch(const char *key);
	int Delete(const char *key);
	void rewind();
}

int DBHANDLE::Store(const char *key, const char *data, STORE_TYPE flag){
	pair<Node<string>*, int> target = tree.search(key);
	bool existed = target.first = NULL;
	if((existed)&&(flag == INSERT)) return 1;
	if((!existed)&&(flag == REPLACE)) return -1;
	else{
		int dataLen = data.size();
		if(!existed){
			bool reuse = reUse(const char *key, const char *data);
			// firstly, try to reuse memory released before
			if(!reuse){
				int maxOffSet = filledMry->offset + filledMry->len + 1;
				mrySpace new_space = new mrySpace(maxOffSet, dataLen);
				new_space->nextSpace = filledMry;
				filledMry->preSpace = new_space;
				filledMry = new_space;
			}
		}
		else{
			Node<string>* dataNode = target.first;
			if(dataNode->data[target.second]->len >= dataLen){
				// memory enough for overwrite
				dataNode->data[target.second]->len = dataLen;
				int offset = dataNode->data[target.second]->offset;
				mrySpace *mryBlock = filledMry;
			}
			else{
				// former memory space is not enough for new data
				int maxOffSet = filledMry->offset + filledMry->len + 1;
				mrySpace *mryBlock = filledMry;
				while(mryBlock != NULL){
					// throw former memory space into the empty space link
					if(mryBlock->offset == offset){
						if(mryBlock->preSpace) mryBlock->preSpace->nextSpace = mryBlock->nextSpace;
						if(mryBlock->nextSpace) mryBlock->nextSpace->preSpace = mryBlock->preSpace;
						if((!mryBlock->preSpace)&&(!mryBlock->nextSpace)) filledMry = NULL;
						mryBlock->preSpace = NULL;
						mryBlock->nextSpace = emptyMry;
						emptyMry->preSpace = mrySpace;
						emptyMry = mryBlock;
					}
				}
				dataNode->data[target.second]->len = dataLen;
				dataNode->data[target.second]->offset = maxOffSet;
				rySpace new_space = new mrySpace(maxOffSet, dataLen);
				new_space->nextSpace = filledMry;
				filledMry->preSpace = new_space;
				filledMry = new_space;
			}
		}
		return 0;
	}
}

char *DBHANDLE::Fetch(const char *key){
	pair<Node<string>* , int> target = tree.search(key);
	
}

DBHANDLE db_open(const char *pathname, OPEN_TYPE oflag){
	string idxfile = pathname + ".idx";
	string datfile = pathname + ".dat";
	fstream = input_index;
	fstream = input_data;
	if(oflag != CREATE){
		if(input_index.open(idxfile, ios::in))&&(input_data.open(datfile, ios::in)){
		// the index file and data has existed
		return DBHANDLE(pathname, input_index, input_data, oflag);
		}
		else error("can't find and open index file and data file.");
	}
	if(oflag == CREATE){
		return	DBHANDLE(pathname, oflag);
	}
}

void db_close(DBHANDLE db){
	db.index_file.close();
	db.data_file.close();
}


int db_store(DBHANDLE db, const char *key, const char *data, STORE_TYPE flag){
	return db.Store(key, data, flag);
}


char *db_fetch(DBHANDLE db, const char *key){
	return db.Fetch(key);
}


int db_delete(DBHANDLE db, const char *key);

void db_rewind(DBHANDLE db){
	db.hook = db.header;
	while(db.hook != NULL){
		char *data = db_nextrec(db, key);
		for(int index = 0; index < db.hook->keys.size(); index++){
			char *key = db.hook->keys[index];
			char *data = db.Fetch(key);
			cout << data;
		}
	}
}

char *db_nextrec(DBHANDLE db, char *key){

}
