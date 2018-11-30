/*	
	Overview:
	File defining a database to do common operation on data, such as inserting, replacing,
	deleting, fetching data record and so on. The data structure, based on which the database
	is constructed, is, be default b+ tree. The definition and relization of b+ tree can be found
	in file 'BPlusTree.h'. The database is de fault to support <string, string> record for 
	convenience, this can be easily expanded by using template programming.

	Database mode:
	The database building experienced a long exploration. Concerning the size of index file, 
	reading it into memory totally once may meet obstace because of hardware capacity and operation
	system limit. So two mode provided on realizing the database:
		Mode A: when index file is not too large, can do all operations in the memory, and only
			rewrite the index file when closing the database, this mode shows a good performance 
			on time lantency, but may meet disablity when too much record needed for operating;
		Mode B: when index is too large, do all operations and reading all index into memory is
			not practical, so many operations should be done in memory and file seperatedly, thus,
			synchronization between record in memory and that in file can't be kept all the time
	Note that resulting from the concern on performance for further analysis and evaluation, mode 
	A is chosen by de fault. What's more, the realization of mode B have different methods, which
	are not always comptible with mode A on the structure of .idx file. More information could be 
	found in provided document. And some code for the realization of mode B is showed in this file 
	already.
*/

#include"bplustree.h"
#include<string>
#include<fstream>
#include<cstring>
#include<sstream>
#include<map>

using namespace std;

const int cacheSize = 10;	// the max size of cache
const int bufferSize = 100;
const int maxRecInMry = 100000000; // max number of record in memory simultaneously

enum STORE_TYPE { INSERT, REPLACE, STORE };
enum OPEN_TYPE { READ, WRITE, CREATE };
enum DB_MODE { A, B};

const char SPERATOT = ';';	// character to seperate two index information
const char SPACE = ' ';		// character to seperate different parts in an index

class Token{
public:
	// class to record the modification of data in memory
	Token(string Key, string Data, int Pos, bool deletion = false){
		key = Key;
		data = Data;
		pos = Pos;
		deleted = deletion;
	}
	string key;
	string data;
	int pos;	// the pos to write new data record
	bool newSpace; // whether write modified data at the end of data file
	bool deleted; // show whether the operation on modNode is deletion
};

class mrySpace{
	// class defined, which is the memory space used to store index information
	// note that this class is important for the reuse of memory space
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
	// class of a database handle
public:
	DBHANDLE(char *pathname, OPEN_TYPE oflag, DB_MODE mode);
	Tree* tree;		// the data structure of database, default a b plus tree
	Node* header;	// pointer to the first leaf node in tree
	Node* hook;		// a reading iterator to traverse all leaf nodes
	mrySpace *filledMry;	// head of chain of memory space storing index information
	mrySpace *emptyMry;		// head of chain of memory space not being used
	string filename;
	OPEN_TYPE open_type;
	fstream index_file;
	fstream data_file;
	int Store(string key, string data, STORE_TYPE flag);
	string Fetch(string key);
	int Delete(string key);
	void rewind();
	string nextrec(string key);
	bool reUse(string key, string data);
	void write_idx(DB_MODE); // edit idx file when close the db
	vector<pair<string, string>> cache;	// a variable defined to store information watched lately
	map<string, Token*> buffer;
	void addToken(string key, string data, int pos, bool deleted);
	void updateData();	// hold records changed lately with a threshold of number
};

void DBHANDLE::addToken(string key, string data, int pos, bool deleted = false){
	// updata the information of record into the buffer
	bool inBuffer = buffer.count(key) != 0;
	if(inBuffer){
		if (deleted){
			buffer[key]->deleted = true;
			return;
		}
		buffer[key]->data = data;
		buffer[key]->pos = pos;
		return;
	}
	else{
		if (deleted){
			buffer[key] = new Token(key, "", -1, true);
			return;
		}
		buffer[key] = new Token(key, data, pos);
		return;
	}
}

void DBHANDLE::updateData(){
	// updata the record in buffer into the data file
	for(map<string, Token*>::iterator it = buffer.begin(); it != buffer.end(); it++){
		Token* token = it->second;
		if(!token->deleted){
			data_file.seekp(token->pos);
			data_file << token->data;
		}
	}
	buffer.clear(); // clear all elements in buffer
}

DBHANDLE::DBHANDLE(char *pathname, OPEN_TYPE oflag, DB_MODE mode = A) {
	open_type = oflag;
	filename = pathname;
	string idx = pathname;
	idx += ".idx";
	string dat = pathname;
	dat += ".dat";
	if (oflag == CREATE) {
		// create a new database with new index and data file
		index_file.open(idx, ios::out);
		data_file.open(dat, ios::out);
	}
	if (oflag == READ) {
		// create a new database with existed file 
		// and can only read inside information
		index_file.open(idx, ios::in);
		data_file.open(dat, ios::in);
	}
	if (oflag == WRITE) {
		// full permission on both reading and writing operation
		// on database, if index and data file hasn't exited 
		// already, create two empty file
		index_file.open(idx, ios::out|ios::in);
		data_file.open(dat, ios::out|ios::in);
		if ((!index_file) || (!index_file)){
			index_file.open(idx, ios::out);
			data_file.open(dat, ios::out);
			index_file.close();
			data_file.close();
			index_file.open(idx, ios::out | ios::in);
			data_file.open(dat, ios::out | ios::in);
		}
	}
	if (!index_file) cout << "opening index file failed!" << endl;
	if (!data_file) cout << "opening data file failed" << endl;
	tree = new Tree();
	filledMry = NULL;
	emptyMry = NULL;
	hook = NULL;
	index_file.seekg(0);
	data_file.seekg(0);
	if (oflag != CREATE) {
		int readNum = 0;
		if (mode != B){
			// if mode = A or B, should pre construct a tree with large 
			// record at advance
			string key;
			string len;
			string offset;
			while (!index_file.eof()){
				// traverse all stored index information in .idx file
				index_file >> key;
				if (key.size() == 0) break;
				index_file >> offset;
				index_file >> len;
				len.erase(len.end() - 1);
				tree->insert(key, indexing(atoi(offset.c_str()), atoi(len.c_str())));
				mrySpace* new_space = new mrySpace(atoi(offset.c_str()), atoi(len.c_str()));
				if (filledMry != NULL) filledMry->preSpace = new_space;
				new_space->nextSpace = filledMry;
				filledMry = new_space;
				key = "";
				len = "";
				offset = "";
				if (mode == B){
					readNum++;
					if (readNum == maxRecInMry) break;
				}
			}
			header = tree->leafHeader;
			hook = tree->leafHeader;
		}
	}
}

void DBHANDLE::write_idx(DB_MODE = A){
	// write index information from the memory into .idx file
	index_file.clear();
	Node* header = tree->leafHeader;
	while(header != NULL){
		for(int i = 0; i < header->keys.size(); i++){
			index_file << SPACE << header->keys[i];
			index_file << SPACE;
			index_file << header->data[i].offset;
			index_file << SPACE;
			index_file << header->data[i].len;
			index_file << SPERATOT;
		}
		header = header->nextLeaf;
	}
	return;
}

void DBHANDLE::rewind(){
	// rewind the reading header of index to the first record
	hook = tree->leafHeader;
}

string DBHANDLE::nextrec(string key){
	// return next record following given key
	// if fail, return a empty stiring
	pair<Node*, int> target = tree->search(key);
	Node* dataNode = target.first;
	int index = target.second;
	if(dataNode == NULL) return "";
	else{
		if(dataNode->nextLeaf == NULL){
			// reading header has come to the last record
			if(index == dataNode->keys.size() - 1) return "";
			else{
				return dataNode->keys[index + 1];
			}
		}
		else{
			if (index == dataNode->keys.size() - 1){
				return dataNode->nextLeaf->keys[0];
			}
			else{
				return dataNode->keys[index + 1];
			}
		}
	}

}

bool DBHANDLE::reUse(string key, string data){
	// try the reuse empty memory space
	// return the result the reuse, if true, successfully reuse
	int needLen = data.size();
	mrySpace* header = emptyMry;
	while(header != NULL){
		if(header->len >= needLen){
			// if target memory space has enough space, can reuse it
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
			addToken(key, data, pos);
			if(buffer.size() > bufferSize) updateData();
			// data_file.seekg(pos);	// overwrite data information
			// data_file << data;
			return true;
		}
		header = header->nextSpace;
	}
	return false;
}

int DBHANDLE::Store(string key, string data, STORE_TYPE flag){
	/* 
		store key and data into database. Three mode to be chosen, if 'INSERT', only can
		insert a index not stored before; if 'REPLACE', can only replace index information
		alread stored; if 'STORE', can perform both inserting and replacing operations, 
		referring to whether the key has alread stored before.
	*/
	if (buffer.count(key) != 0){
		if (buffer[key]->deleted){
			if (flag == REPLACE) return -1;
			else{
				buffer.erase(key);
			}
		}
		else{
			if (flag == INSERT) return -1;
		}
	}
	pair<Node*, int> target = tree->search(key);
	bool existed = target.first != NULL;
	if((existed)&&(flag == INSERT)) return -1;
	if((!existed)&&(flag == REPLACE)) return -1;
	else{
		int dataLen = data.size();
		if(!existed){
			// cout << "not existed" << endl;
			bool reuse = false;
			// bool reuse = reUse(key, data);
			// firstly, try to reuse memory released before
			if(!reuse){
				int maxOffSet;
				if (filledMry) {
					maxOffSet = filledMry->offset + filledMry->len;
				}
				else maxOffSet = 0;
				mrySpace* new_space = new mrySpace(maxOffSet, dataLen);
				new_space->nextSpace = filledMry;
				if (filledMry) filledMry->preSpace = new_space;
				filledMry = new_space;
				int len = data.size();
				tree->insert(key, indexing(maxOffSet, len));
				addToken(key, data, maxOffSet);
				if(buffer.size() > bufferSize) updateData();
				// data_file.seekp(maxOffSet);
				// data_file << data;
			}
		}
		else{
			// cout << "existed" << endl;
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
				addToken(key, data, offset);
				if(buffer.size() > bufferSize) updateData();
				// data_file.seekp(offset);
				// data_file << data;
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
						if ((!mryBlock->preSpace) && (!mryBlock->nextSpace)){
							filledMry = NULL;
						}
						mryBlock->preSpace = NULL;
						mryBlock->nextSpace = emptyMry;
						if(emptyMry) emptyMry->preSpace = mryBlock;
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
				new_space->preSpace = NULL;
				filledMry = new_space;
				addToken(key, data, maxOffSet);
				if(buffer.size() > bufferSize) updateData();
				// data_file.seekp(maxOffSet);
				// data_file << data;
			}
		}
		return 0;
	}
}

string DBHANDLE::Fetch(string key){
	if (buffer.count(key) != 0){
		if (buffer[key]->deleted) return "";
		else return buffer[key]->data;
	}
	// firstly, check the buffer for searching key
	pair<Node* , int> target = tree->search(key);
	// if key doesn't be found in cache buffer, search it in tree
	if(!target.first) return "";
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
	// delete the key stored in database
	if (buffer.count(key) != 0){
		if (buffer[key]->deleted) return 0;
		else{
			buffer[key]->deleted = true;
			return 0;
		}
	}
	pair<Node*, int> target = tree->search(key);
	if(!target.first) return -1;	// if key doesn't be stored, return
	else{
		int index = target.second;
		Node* dataNode = target.first;
		int offset = dataNode->data[index].offset;
		int len = dataNode->data[index].len;
		mrySpace *header = filledMry;
		while(header != NULL){
			if(header->offset == offset){
				if(header->preSpace) header->preSpace->nextSpace = header->nextSpace;
				if (header->nextSpace){
					header->nextSpace->preSpace = header->preSpace;
					if (header == filledMry) filledMry = header->nextSpace;
				}
				if ((!header->preSpace) && (!header->nextSpace)){
					filledMry = NULL;
				}
				header->preSpace = NULL;
				header->nextSpace = emptyMry;
				if(emptyMry) emptyMry->preSpace = header;
				emptyMry = header;
				break;
			}
			header = header->nextSpace;
		}
		addToken(key, "", -1, true);
		if (buffer.size() > bufferSize) updateData();
		tree->remove(key);
		return 0;
	}
}

DBHANDLE* db_open(char *pathname, OPEN_TYPE oflag, DB_MODE mode = A){
	// create a database with given file
	// or totally create a new database with creating .idx and .dat files
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
	return new DBHANDLE(pathname, oflag, mode);
}

void db_close(DBHANDLE* db){
	// close database
	db->write_idx();	// write the new .idx file
	db->index_file.close();
	db->updateData();	// update record in .data file
	db->data_file.close();
}


int db_store(DBHANDLE* db, string key, string data, STORE_TYPE flag){
	// store index and data information into database with certain mode
	return db->Store(key, data, flag);
}


string db_fetch(DBHANDLE* db, string key){
	// get a data connected with given index
	return db->Fetch(key);
}


int db_delete(DBHANDLE* db, string key){
	// delete record in database with given index
	return db->Delete(key);
}

void db_rewind(DBHANDLE* db){
	// rewind the reading header of database to its very beginning
	db->rewind();
	return;
}


string db_nextrec(DBHANDLE db, string key){
	// get the next record following fiven index
	return db.nextrec(key);
}
