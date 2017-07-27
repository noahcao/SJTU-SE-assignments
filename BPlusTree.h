#include<iostream>
#include<vector>
#include<string>
#include<sstream>

using namespace std;

const int ORDER = 64;  // order of b+ tree

class indexing{
	// class to hold the information of index
public:
	indexing(int Offset, int Len){
		len = Len;
		offset = Offset;
	}
	int len;	// the length of target data in data file
	int offset;	// the offset of the position of target data in data file
};

template<class elem>
int searchPos(vector<elem> keys, elem key) {
    // function to indicate pos of insertion
	int size = keys.size();
	for (int i = 0; i < size; i++) {
		if (key <= keys[i]) return i;
	}
	return size;
}

template<class elem>
void insertElem(vector<elem> &vect, int pos, elem key){
	int formerlen = vect.size();
    // insert a element into a vector according to given position
    vector<elem> new_vec;
    for(int i = 0; i < pos; i ++){
        new_vec.push_back(vect[i]);
    }
    new_vec.push_back(key);
    for(int i = pos; i < vect.size(); i++){
        new_vec.push_back(vect[i]);
    }
	vect = new_vec;
}

template<class elem>
void removeElem(vector<elem> &vect, int pos){
	int formerlen = vect.size();
    // remove a certain element in the vector
	vector<elem> new_vec;
	for(int i = 0; i < pos; i++) new_vec.push_back(vect[i]);
	for(int i = pos + 1; i < vect.size(); i++) new_vec.push_back(vect[i]);
	vect = new_vec;
};


class Node{
    // class of node of bplustree
public:
	Node(){
        // construct a totally empty node
		parent = nullptr;
		vector<string> keys;
		vector<Node*> children;
        isLeaf = false;
	}
    Node(string key, indexing Data){
		// construct a data with index information
		preLeaf = NULL;
		nextLeaf = NULL;
        isLeaf = true;
        vector<indexing> data;
        vector<string> keys;
        keys.push_back(key);
        data.push_back(Data);
    }
	int getKeyNum(){return keys.size();}
	bool isLeaf;	// whether this node is a leaf node
	Node* nextLeaf;	// previous leaf of leaf node in the leaf linker
	Node* preLeaf;	// next leaf of leaf node in the leaf linker
	vector<string> keys;
	Node* parent;
	vector<indexing> data;
	vector<Node*> children;
};

class Tree{
	// class of the B+ tress
protected:
	int _size;  // size of tree
	int _order; // parameter of the order of tree
	Node* _root; // pointer of the root node
	Node* _hook; // pointer of the node which is last one searched
	void overFlow(Node*);
	void underFlow(Node*);
public:
	Tree(int order = ORDER): _order(order)
	{
		_root = new Node();
        _root->isLeaf = true;
		leafHeader = NULL;
	}
	~Tree(){if(_root) delete(_root);}
	Node* leafHeader;
	Node* root(){return _root;}
	bool empty(){return !_root;}
	pair<Node*, int> search(const string key);
	indexing getData(string key);
	bool insert(const string key, indexing Data);
	bool remove(const string key);
    Node* mergeChild(Node* ln, Node* rn);
    void printTree();
};

indexing Tree::getData(string key) {
	// get indexing information of key
	pair<Node*, int> target = search(key);
	if (!target.first) return indexing(-1, -1);
	else {
		Node* dataNode = target.first;
		int index = target.second;
		return dataNode->data[index];
	}
}

void printNode(Node* node){
	// print a node key
    if(node->keys.size() == 0) return;
    if(node->isLeaf){
		cout << "   leaf   ";
        for(int i = 0; i < node->keys.size(); i++){
            cout << node->keys[i] << " ";
        }
        cout << "     ";
    }
    else{
		cout << "   not leaf   ";
        for(int i = 0; i < node->keys.size(); i++){
            cout << node->keys[i] << " ";
        }
        cout << endl;
        for(int i = 0; i < node->children.size(); i++){
            printNode(node->children[i]);
        }
    }
}

void Tree::printTree() {
	// print the structure of tree
	// this function is mainly designed for checking 
	// whether tree's structure is right
	cout << endl;
    if(!_root) return;
    else printNode(_root);
	cout << endl;
	cout << "print the leaves! " << endl;
	Node* header = leafHeader;
	while (header){
		for (int i = 0; i < header->keys.size(); i++){
			cout << header->keys[i] << "  ";
		}
		header = header->nextLeaf;
	}
}

Node* Tree::mergeChild(Node *ln, Node *rn) {
    // function to merge two neighbor children
	if (ln->isLeaf) {
		for (int i = 0; i < rn->keys.size(); i++) {
			ln->keys.push_back(rn->keys[i]);
			ln->data.push_back(rn->data[i]);
		}
		ln->nextLeaf = rn->nextLeaf;
		if(ln->nextLeaf) ln->nextLeaf->preLeaf = ln;
		return ln;
	}
	else {
		Node* mergeNode = mergeChild(ln->children.back(), rn->children[0]);
		rn->children[0]->parent = NULL;
		delete(rn->children[0]);
		removeElem(rn->children, 0);
		for (int i = 0; i < rn->keys.size(); i++){
			ln->keys.push_back(rn->keys[i]);
			ln->children.push_back(rn->children[i]);
			rn->children[i]->parent = ln;
		}
		return mergeNode;
	}
}

pair<Node*, int> Tree::search(const string key) {
    // search the key among the leaves of tree
	Node* node = _root;
	_hook = NULL;
	while(node){
		if(node->isLeaf){
			vector<string> keys = node->keys;
			int keyNum = keys.size();
			for(int i = 0; i < keyNum; i++) {
				if (keys[i] == key) return pair<Node*, int>(node, i);
			}
			return pair<Node*, int>(NULL, -1);
		}
		else{
			vector<string> keys = node->keys;
			int keyNum = keys.size();
			int Index = 0;
			while(Index < keyNum){
				if(key < keys[Index]){
                    _hook = node;
					node = node->children[Index];
					break;
				}
				if(Index == keyNum - 1){
                    _hook = node;
					node = node->children[Index + 1];
					break;
				}
				Index++;
			}
		}
	}
	return pair<Node*, int>(NULL, -1);    // if key not found among leaves, return NULL
}

bool Tree::insert(const string key, indexing Data){
	// insert a key with index information into the tree
	pair<Node*, int> target = this->search(key);
    Node* node = target.first;
    if(node) return false;
	// if the key already existing, do nothing
	node = new Node(key, Data);
    if(_hook == NULL){
        // case that leaf is root
		node = _root;
		int index = searchPos(node->keys, key);
        insertElem(node->keys, index, key);
        insertElem(node->data, index, Data);
		if (_root->keys.size() == 1){
			leafHeader = _root;
			_root->nextLeaf = NULL;
			_root->preLeaf = NULL;
		}
        overFlow(node);
    }
    else{
        // case when depth of tree is bigger than 1
        // so, _hook would point to the parent of the leaf that key should be inserted into
        int rank = searchPos(_hook->keys, key);
        Node* leaf = _hook->children[rank];
        int insertpos = searchPos(leaf->keys, key);
        insertElem(leaf->keys, insertpos, key);
        insertElem(leaf->data, insertpos, Data);
        overFlow(leaf);
    }
}

void Tree::overFlow(Node* node) {
	// to settle the overflow of node
	// note that this function is often called recursively, which
	// finally ends at the root node
	if (_order >= node->keys.size()) return;
	int mid = (_order + 1) / 2;
	string insertKey = node->keys[mid];
	Node* new_node = new Node();
    if(node->isLeaf){
        // case 1: node is leaf, the mid key should be copied to parent node
		new_node->isLeaf = true;
		new_node->preLeaf = NULL;
		new_node->nextLeaf = NULL;
        //for(int i = 0; i < _order - mid + 1; i++){
		int size = node->keys.size();
		for (int i = 0; i < size - mid; i++) {
            new_node->keys.push_back(node->keys[mid]);
			new_node ->data.push_back(node->data[mid]);
            removeElem(node->keys, mid);
			removeElem(node->data, mid);
        }
		new_node->nextLeaf = node->nextLeaf;
		if (new_node->nextLeaf) new_node->nextLeaf->preLeaf = new_node;
		node->nextLeaf = new_node;
		new_node->preLeaf = node;
		if (node->parent) {
			new_node->parent = node->parent;
			int index = searchPos(node->parent->keys, insertKey);
			insertElem(node->parent->keys, index, insertKey);
			insertElem(node->parent->children, index + 1, new_node);
			overFlow(node->parent);
		}
		else {
			Node* new_root = new Node();
			_root = new_root;
			_root->children.push_back(node);
			_root->children.push_back(new_node);
			_root->keys.push_back(insertKey);
			node->parent = _root;
			new_node->parent = _root;
		}
    }
    if(!node->isLeaf){
        // case 2: node is not leaf, only promote the mid key to parent node
		int size = node->keys.size();
        //for(int i = 0; i < _order - mid; i++){
		for(int i = 0; i < size - mid - 1; i++){
            new_node->children.push_back(node->children[mid + 1]);
			node->children[mid + 1]->parent = new_node;
            new_node->keys.push_back(node->keys[mid + 1]);
            removeElem(node->children, mid + 1);
            removeElem(node->keys, mid + 1);
        }
        new_node->children.push_back(node->children.back());
		node->children.back()->parent = new_node;
        removeElem(node->children, mid + 1);
        removeElem(node->keys, mid);	// remove the insert key from former node
		if (node == _root) {
			// case when node is the root (has no parent)
			Node* new_root = new Node();
			_root = new_root;
			new_node->parent = _root;
			node->parent = _root;
			_root->keys.push_back(insertKey);
			_root->children.push_back(node);
			_root->children.push_back(new_node);
		}
		else {
			// case when node has a parent
			new_node->parent = node->parent;
			int index = searchPos(node->parent->keys, insertKey);
			insertElem(node->parent->keys, index, insertKey);
			insertElem(node->parent->children, index + 1, new_node);
			overFlow(node->parent);
		}
    }
}

bool Tree::remove(const string key) {
	// remove a key from the tree
	// note that the deletion is always begins from a leaf node
	// in many cases, a internal node holding same key will also be deleted
	pair<Node*, int> target = search(key);
	Node* node = target.first;
	int index = target.second;
	if (!node) return false;
	// if key exists, then node should be a leaf 
    // case 0: node is the root (tree with height 1)
    if (node == _root){
        removeElem(node->keys, index);
		removeElem(node->data, index);
        return true;
    }
    // case 1: index != 0
    if (index != 0){
        removeElem(node->keys, index);
		removeElem(node->data, index);
		underFlow(node);
        return true;
    }
	if (index == 0) {
		if (node == node->parent->children[0]) {
			// case 2: index == 0, node is the first child of parent;
			// and node has more than one key
			removeElem(node->keys, 0);
			removeElem(node->data, 0);
			Node* formerNode = node;
			// if node is the first child of parent, then its ancesstor may also
			// have the same key
			while (node == node->parent->children[0]) {
				node = node->parent;
				if (node == _root){
					underFlow(formerNode);
					return true;
				}
			}
			node = node->parent;
			int index = searchPos(node->keys, key);
			if (node == _root) {
				if (node->keys.size() == 1) {
					Node* flow_node = mergeChild(node->children[0], node->children[1]);
					delete(node->children[1]);
					removeElem(node->children, 1);
					node->children[0]->parent = NULL;
					_root = node->children[0];
					removeElem(node->children, 0);
					delete(node);
					while (flow_node != _root) {
						overFlow(flow_node);
						flow_node = flow_node->parent;
					}
					overFlow(_root);
				}
				else {
					removeElem(node->keys, index);
					Node* flow_node = mergeChild(node->children[index], node->children[index + 1]);
					node->children[index + 1]->parent = NULL;
					delete(node->children[index + 1]);
					removeElem(node->children, index + 1);
					while (flow_node != _root) {
						overFlow(flow_node);
						flow_node = flow_node->parent;
					}
					overFlow(_root);
				}
			}
			else {
				removeElem(node->keys, index);
				Node* flow_node = mergeChild(node->children[index], node->children[index + 1]);
				node->children[index + 1]->parent = NULL;
				delete(node->children[index + 1]);
				removeElem(node->children, index + 1);
				while (flow_node != _root) {
					overFlow(flow_node);
					flow_node = flow_node->parent;
				}
				underFlow(node);
			}
			underFlow(formerNode);
		}
		else{
			// case 3: index == 0, node isn't the first child of parent;
			int rank = searchPos(node->parent->keys, key);
			node->parent->keys[rank] = node->keys[1];
			removeElem(node->keys, 0);
			removeElem(node->data, 0);
			underFlow(node);
		}
		return true;
    }
}

void Tree::underFlow(Node* node) {
	// settle the underflow of node
	// note that this function is often called recursively 
	// which is finally ends at the root node
	if ((_order + 1) / 2 <= node->keys.size() + 1) {
		return;
	}
	Node* pNode = node->parent;
	if(!pNode){
		if (!node->isLeaf) {
			if ((node->keys.size() == 0) && (node->children.size() > 0)) {
				// case that the root (node) has no key already but has a sole child
				// which is not empty
				_root = node->children[0];
				_root->parent = NULL;
				removeElem(node->children, 0);
				delete node;
			}
		}
		return;
	}
	int rank = 0;
	while (pNode->children[rank] != node) rank++;
	// case 1: borrow a key from the left subling
	if(0 < rank){
		// if node is not the first child of pNode
		// then node certainly has a left subling
		Node* ls = pNode->children[rank - 1];
		if((_order + 1) / 2 < ls->keys.size() + 1){
			if (ls->isLeaf) {
				pNode->keys[rank - 1] = ls->keys.back();
				insertElem(node->keys, 0, ls->keys.back());
				removeElem(ls->keys, ls->keys.size() - 1);
				insertElem(node->data, 0, ls->data.back());
				removeElem(ls->data, ls->data.size() - 1);
			}
			else {
				insertElem(node->keys, 0, pNode->keys[rank - 1]);
				pNode->keys[rank - 1] = ls->keys.back();
				removeElem(ls->keys, ls->keys.size() - 1);
				insertElem(node->children, 0, ls->children.back());
				removeElem(ls->children, ls->children.size() - 1);
				node->children[0]->parent = node;
			}
			return;
		}
	}
	// case 2: borrow a key from the right subling
	if(pNode->children.size() - 1 > rank) {
        // if node is not the last child of pNode
        // then node certainly has a right subling
        Node* rs = pNode->children[rank + 1];
        if ((_order + 1) / 2 < rs->keys.size() + 1) {
			if (!node->isLeaf) {
				node->keys.push_back(pNode->keys[rank]);
				pNode->keys[rank] = rs->keys[0];
				removeElem(rs->keys, 0);
				node->children.push_back(rs->children[0]);
				removeElem(rs->children, 0);
				node->children.back()->parent = node;
			}
			else {
				pNode->keys[rank] = rs->keys[1];
				node->keys.push_back(rs->keys[0]);
				node->data.push_back(rs->data[0]);
				removeElem(rs->keys, 0);
				removeElem(rs->data, 0);
			}
            return;
        }
    }
    // case 3: right subling and left subling are empty or
    // dosen't have enough children to be given
    // so, we merge a subling with the node
    if(0 < rank){
        // merge node with its left subing
        Node* ls = pNode->children[rank - 1];
		if (!node->isLeaf) {
			ls->keys.push_back(pNode->keys[rank - 1]);
			ls->children.push_back(node->children[0]);
			removeElem(node->children, 0);
			ls->children.back()->parent = ls;
			while (!node->keys.empty()){
				ls->keys.push_back(node->keys[0]);
				removeElem(node->keys, 0);
				ls->children.push_back(node->children[0]);
				removeElem(node->children, 0);
				ls->children.back()->parent = ls;
			}
		}
		else{
			while (!node->keys.empty()){
				ls->keys.push_back(node->keys[0]);
				removeElem(node->keys, 0);
				ls->data.push_back(node->data[0]);
				removeElem(node->data, 0);
			}
			ls->nextLeaf = node->nextLeaf;
			if (ls->nextLeaf) ls->nextLeaf->preLeaf = ls;
		}
		removeElem(pNode->keys, rank - 1);
		removeElem(pNode->children, rank);
        delete(node);
    }
    else{
        // merge node with its right subling
        Node* rs = pNode->children[rank + 1];
		if (!node->isLeaf) {
			insertElem(rs->keys, 0, pNode->keys[rank]);
			insertElem(rs->children, 0, node->children.back());
			removeElem(node->children, node->children.size() - 1);
			rs->children[0]->parent = rs;
			while (!node->keys.empty()){
				insertElem(rs->keys, 0, node->keys.back());
				removeElem(node->keys, node->keys.size() - 1);
				insertElem(rs->children, 0, node->children.back());
				removeElem(node->children, node->children.size() - 1);
				rs->children[0]->parent = rs;
			}
		}
		else{
			insertElem(rs->keys, 0, node->keys.back());
			removeElem(node->keys, node->keys.size() - 1);
			insertElem(rs->data, 0, node->data.back());
			removeElem(node->data, node->data.size() - 1);
			if (leafHeader == node){
				leafHeader = rs;
				leafHeader->preLeaf = NULL;
			}
			else{
				rs->preLeaf = node->preLeaf;
				node->preLeaf->nextLeaf = rs;
			}
		}
		removeElem(pNode->keys, rank);
		removeElem(pNode->children, rank);
        delete(node);
    }
    underFlow(pNode);
    return;
}
