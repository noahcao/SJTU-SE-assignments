#include<iostream>
#include<vector>
#include<string>

using namespace std;

const int ORDER = 3;  // order of b+ tree

/*class idx{
public:
	idx(int offSet, int len, idx* nextIdx = NULL, idx* previousIdx = NULL){
		offset = offSet;
		dattalen = len;
		next = nextIdx;
		previous = previousIdx;
	}
	int offset;
	int dattalen;
	idx* next;
	idx* previous;
};*/

template<typename T>
int searchPos(vector<T> keys, T key) {
    // function to indicate pos of insertion
	int size = keys.size();
	for (int i = 0; i < size; i++) {
		if (key <= keys[i]) return i;
	}
	return size;
}

template<class elem>
void insertElem(vector<elem> &vect, int pos, elem key){
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
    // remove a certain element in the vector
	vector<elem> new_vec;
	for(int i = 0; i < pos; i++) new_vec.push_back(vect[i]);
	for(int i = pos + 1; i < vect.size(); i++) new_vec.push_back(vect[i]);
	vect = new_vec;
}

template <typename T>
class Node{
    // class of node of bplustree
public:
	Node(){
        // construct a totally empty node
		parent = nullptr;
		vector<T> keys;
		vector<Node<T>*> children;
        isLeaf = false;
	}
    Node(T key, string Data){
		preLeaf = NULL;
		nextLeaf = NULL;
        isLeaf = true;
        vector<string> data;
        vector<T> keys;
        keys.push_back(key);
        data.push_back(Data);
    }
	Node(T key, Node<T>* lc = NULL, Node<T>* rc = NULL){
        // construct a new node with left child and right child
		vector<T> keys;
		vector<Node<T>*> children;
		parent = NULL;
		keys.push_back(key);
		children.push_back(lc);
		children.push_back(rc);
        isLeaf = false;
		if(lc) lc->parent = this;
		if(rc) rc->parent = this;
	}
	int getKeyNum(){return keys.size();}
	bool isLeaf;
	Node<T>* nextLeaf;
	Node<T>* preLeaf;
	vector<T> keys;
	Node<T>* parent;
	vector<string> data;
	vector<Node<T>*> children;
};

template<class elem>
void insertElem(vector<Node<elem>*> &vect, int pos, elem key){
    // reload the function insertElem, make it support the insertion
    // of element into vector of pointers of nodes
    vector<Node<elem>*> new_vec;
    for(int i = 0; i < pos; i ++){
        new_vec.push_back(vect[i]);
    }
    new_vec.push_back(key);
    for(int i = pos; i < vect.size(); i++){
        new_vec.push_back(vect[i]);
    }
    vect = new_vec;
}


template <typename T>
class Tree{
protected:
	int _size;  // size of tree
	int _order; // parameter of the order of tree
	Node<T>* _root; // pointer of the root node
	Node<T>* _hook; // pointer of the node which is last one searched
	void overFlow(Node<T>*);
	void underFlow(Node<T>*);
public:
	Tree(int order = ORDER): _order(order), _size(0)
	{
		_root = new Node<T>();
        _root->isLeaf = true;
		leafHeader = NULL;
	}
	~Tree(){if(_root) delete(_root);}
	int const order(){return _order;}
	int const size(){return _size;}
	Node<T>* leafHeader;
	Node<T>* root(){return _root;}
	bool empty() const {return !_root;}
	pair<Node<T>*, int> search(const T& key);
	bool insert(const T& key, string Data);
	bool remove(const T& key);
    Node<T>* mergeChild(Node<T>* ln, Node<T>* rn);
    void printTree();
    void clear();
};

template <typename T>
void deleteTree(Node<T>* node){
    if(node->isLeaf) delete(node);
    else{
        for(int i = 0; i < node->children.size(); i++){
            deleteTree(node->children[i]);
        }
    }
}

template<typename T>
void Tree<T>::clear() {
    // function of clear all node in a tree
    if(!_root) return;
    else{
        deleteTree(_root);
    }
}

template<typename T>
void printNode(Node<T>* node){
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

template<typename T>
void Tree<T>::printTree() {
	cout << endl;
    if(!_root) return;
    else printNode(_root);
	cout << endl;
	cout << "print the leaves! " << endl;
	Node<T>* header = leafHeader;
	while (header){
		for (int i = 0; i < header->keys.size(); i++){
			cout << header->keys[i] << "  ";
		}
		header = header->nextLeaf;
	}
}

template<typename T>
Node<T>* Tree<T>::mergeChild(Node<T> *ln, Node<T> *rn) {
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
		Node<T>* mergeNode = mergeChild(ln->children.back(), rn->children[0]);
		removeElem(rn->children, 0);
		for (int i = 0; i < rn->keys.size(); i++){
			ln->keys.push_back(rn->keys[i]);
			ln->children.push_back(rn->children[i]);
			rn->children[i]->parent = ln;
		}
		return mergeNode;
	}
}

template <typename T>
pair<Node<T>*, int> Tree<T>::search(const T &key) {
    // search the key among the leaves of tree
	Node<T>* node = _root;
	_hook = NULL;
	while(node){
		if(node->isLeaf){
			vector<T> keys = node->keys;
			int keyNum = keys.size();
			for(int i = 0; i < keyNum; i++) {
				if (keys[i] == key) return pair<Node<T>*, int>(node, i);
			}
			return pair<Node<T>*, int>(NULL, -1);
		}
		else{
			vector<T> keys = node->keys;
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
	return pair<Node<T>*, int>(NULL, -1);    // if key not found among leaves, return NULL
}

template <typename T>
bool Tree<T>::insert(const T &key, string Data){
	pair<Node<T>*, int> target = this->search(key);
    Node<T>* node = target.first;
    if(node) return false;
	node = new Node<T>(key, Data);
    if(_hook == NULL){
        // case that leaf is root
		node = _root;
        insertElem(node->keys, searchPos(node->keys, key), key);
        insertElem(node->data, searchPos(node->data, Data), Data);
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
        Node<T>* leaf = _hook->children[rank];
        int insertpos = searchPos(leaf->keys, key);
        insertElem(leaf->keys, insertpos, key);
        insertElem(leaf->data, insertpos, Data);
        overFlow(leaf);
    }
}

template <typename T>
void Tree<T>::overFlow(Node<T>* node) {
	if (_order >= node->keys.size()) return;
	int mid = (_order + 1) / 2;
	T insertKey = node->keys[mid];
	Node<T>* new_node = new Node<T>();
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
			Node<T>* new_root = new Node<T>();
			_root = new_root;
			_root->children.push_back(node);
			_root->children.push_back(new_node);
			_root->keys.push_back(insertKey);
			node->parent = _root;
			new_node->parent = _root;
			return;
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
			Node<T>* new_root = new Node<T>();
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

template <typename T>
bool Tree<T>::remove(const T& key) {
	pair<Node<T>*, int> target = search(key);
	Node<T>* node = target.first;
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
			Node<T>* formerNode = node;
			// Node<T>* upHolder = searchUpHolder(key);
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
					Node<T>* flow_node = mergeChild(node->children[0], node->children[1]);
					node->children[0]->parent = NULL;
					_root = node->children[0];
					while (flow_node != _root) {
						overFlow(flow_node);
						flow_node = flow_node->parent;
					}
					overFlow(_root);
				}
				else {
					removeElem(node->keys, index);
					Node<T>* flow_node = mergeChild(node->children[index], node->children[index + 1]);
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
				Node<T>* flow_node = mergeChild(node->children[index], node->children[index + 1]);
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
			return true;
		}
    }
}

template <typename T>
void Tree<T>::underFlow(Node<T> * node) {
	if ((_order + 1) / 2 <= node->keys.size() + 1) {
		return;
	}
	Node<T>* pNode = node->parent;
	if(!pNode){
		if (!node->isLeaf) {
			if ((node->keys.size() == 0) && (node->children[0])) {
				// case that the root (node) has no key already but has a sole child
				// which is not empty
				_root = node->children[0];
				_root->parent = NULL;
				node->children[0] = NULL;
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
		Node<T>* ls = pNode->children[rank - 1];
		if((_order + 1) / 2 < ls->keys.size() + 1){
			insertElem(node->keys, 0, pNode->keys[rank - 1]);
			pNode->keys[rank - 1] = ls->keys[ls->keys.size() - 1];
			removeElem(ls->keys, ls->keys.size() - 1);
			if (ls->isLeaf) {
				insertElem(node->data, 0, ls->data.back());
				removeElem(ls->data, ls->data.size() - 1);
			}
			else {
				insertElem(node->children, 0, ls->children[ls->children.size() - 1]);
				removeElem(ls->children, ls->children.size() - 1);
			}
			if(!node->isLeaf) node->children[0]->parent = node;
			return;
		}
	}
	// case 2: borrow a key from the right subling
	if(pNode->children.size() - 1 > rank) {
        // if node is not the last child of pNode
        // then node certainly has a right subling
        Node<T> *rs = pNode->children[rank + 1];
        if ((_order + 1) / 2 < rs->keys.size() + 1) {
            node->keys.push_back(pNode->keys[rank]);
			if (!node->isLeaf) {
				pNode->keys[rank] = rs->keys[0];
				removeElem(rs->keys, 0);
				node->children.push_back(rs->children[0]);
				removeElem(rs->children, 0);
			}
			else {
				pNode->keys[rank] = rs->keys[1];
				node->data.push_back(rs->data[0]);
				removeElem(rs->keys, 0);
				removeElem(rs->data, 0);
			}
            if(!node->isLeaf) {
                node->children.back()->parent = node;
            }
            return;
        }
    }
    // case 3: right subling and left subling are empty or
    // dosen't have enough children to be given
    // so, we merge a subling with the node
    if(0 < rank){
        // merge node with its left subing
        Node<T>* ls = pNode->children[rank - 1];
        ls->keys.push_back(pNode->keys[rank - 1]);
        removeElem(pNode->keys, rank - 1);
        removeElem(pNode->children, rank);
		if (!node->isLeaf) {
			ls->children.push_back(node->children[0]);
			ls->children.back()->parent = ls;
		}
        while(!node->keys.empty()){
            ls->keys.push_back(node->keys[0]);
            removeElem(node->keys, 0);
			if (!node->isLeaf) {
				ls->children.push_back(node->children[0]);
				removeElem(node->children, 0);
				ls->children.back()->parent = ls;
			}
			else {
				ls->data.push_back(node->data[0]);
				removeElem(node->data, 0);
			}
        }
		if (node->isLeaf){
			ls->nextLeaf = node->nextLeaf;
			if (ls->nextLeaf) ls->nextLeaf->preLeaf = ls;
		}
        delete(node);
    }
    else{
        // merge node with its right subling
        Node<T>* rs = pNode->children[rank + 1];
        insertElem(rs->keys, 0, pNode->keys[rank]);
        removeElem(pNode->keys, rank);
        removeElem(pNode->children, rank);
		if (!node->isLeaf) {
			insertElem(rs->children, 0, node->children.back());
			removeElem(node->children, node->children.size() - 1);
			rs->children[0]->parent = rs;
		}
        while(!node->keys.empty()){
            insertElem(rs->keys, 0, node->keys.back());
            removeElem(node->keys, node->keys.size() - 1);
			if (!node->isLeaf){
				insertElem(rs->children, 0, node->children.back());
				removeElem(node->children, node->children.size() - 1);
			}
			else{
				insertElem(rs->data, 0, node->data.back());
				removeElem(node->data, node->data.size() - 1);
			}
            if(rs->children[0]){
                rs->children[0]->parent = rs;
            }
        }
		if (node->isLeaf){
			rs->preLeaf = node->preLeaf;
			if (rs->preLeaf) rs->preLeaf->nextLeaf = rs;
		}
        delete(node);
    }
    underFlow(pNode);
    return;
}

int main() {
    Tree<int> tree;
	for (int i = 1; i < 31 ; i++) {
		cout << "i = " << i << endl;
		tree.insert(i, to_string(i));
		tree.printTree();
	}
	tree.printTree();
	tree.remove(7);
	tree.printTree();
	tree.remove(1);
	tree.printTree();
	tree.remove(13);
	tree.printTree();
	tree.remove(19);
	tree.printTree();
	system("pause");
}