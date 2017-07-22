#include<iostream>
#include<vector>

using namespace std;

const int ORDER = 4;  // order of b+ tree
// const int MINNUM_KEY = ORDER - 1; // minnum of key number for a node
// const int MAXNUM_KEY = 2 * ORDER - 1; // maxnum of key number for a node
// const int MINNUM_CHILD = MINNUM_KEY + 1; // minnum of children for a node
// const int MAXNUM_CHILD = MAXNUM_KEY + 1; // maxnum of children for a node

template<typename T>
int searchPos(vector<T> keys, T key) {
    // function to indicate pos of insertion
	int size = keys.size();
	for (int i = 0; i < size; i++) {
		if (key < keys[i]) return i;
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
	vector<T> keys;
	vector<Node<T>*> children;
	Node<T>* parent;
    vector<string> data;
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
	}
	~Tree(){if(_root) delete(_root);}
	int const order(){return _order;}
	int const size(){return _size;}
	Node<T>* root(){return _root;}
	bool empty() const {return !_root;}
	pair<Node<T>*, int> search(const T& key);
	bool insert(const T& key, string Data);
	bool remove(const T& key);
    bool recursive_remove(Node<T>* node, T key);
    void mergeNode(Node<T>* ln, Node<T>* rn);
    void downFlow(Node<T>* emptyNode);
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
        for(int i = 0; i < node->keys.size(); i++){
            cout << node->keys[i] << " ";
        }
        cout << "     ";
    }
    else{
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
    cout << "begin print the tree!";
    if(!_root) return;
    else printNode(_root);
}

template<typename T>
void Tree<T>::mergeNode(Node<T> *ln, Node<T> *rn) {
    // function to merge two neighbor nodes
    // be default, the right one would be added into the left node
    if((ln->isLeaf)&&(rn->isLeaf)){
        // nodes to be merged are both leaf nodes
        for(int i = 0; i < rn->keys.size(); i++){
            ln->keys.push_back(rn->keys[i]);
        }
        overFlow(ln);
    }
    else{
        // nodes to be merged have children
        for(int i = 0; i < rn->keys.size(); i++){
            ln->keys.push_back(rn->keys[i]);
        }
        rn->children[0]->parent = ln;
        mergeNode(ln->children.back(), rn->children[0]);
        for(int i = 1; i < rn->children.size(); i++){
            ln->children.push_back(rn->children[i]);
            rn->children[i]->parent = ln;
        }
        overFlow(ln);
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
		}
		else{
			vector<T> keys = node->keys;
			int keyNum = keys.size();
			int Index = 0;
			while(Index <= keyNum){
				if(key < keys[Index]){
                    _hook = node;
					node = node->children[Index];
					break;
				}
				if(Index == keyNum){
                    _hook = node;
					node = node->children[Index];
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
    cout << "sssss";
	pair<Node<T>*, int> target = this->search(key);
    cout << "sssss";
    Node<T>* node = target.first;
    if(node) return false;
    if(_hook == NULL){
        // case that leaf is root
        insertElem(node->keys, searchPos(node->keys, key), key);
        insertElem(node->data, searchPos(node->data, Data), Data);
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
	if(_order >= node->children.size()) return;
	int mid = _order / 2;
	T insertKey = node->keys[mid];
	Node<T>* new_node = new Node<T>();
    if(node->isLeaf){
        // case 1: node is leaf, the mid key should be copied to parent node
        for(int i = 0; i < _order - mid; i++){
            new_node->keys.push_back(node->keys[mid + i]);
            removeElem(node->keys, mid);
        }
        new_node->parent = node->parent;
        new_node->isLeaf = true;
        int index = searchPos(node->parent->keys, insertKey);
        insertElem(node->parent->keys, index, insertKey);
        insertElem(node->parent->children, index + 1, new_node);
        overFlow(node->parent);
    }
    if(!node->isLeaf){
        // case 2: node is not leaf, only promote the mid key to parent node
        for(int i = 0; i < _order - mid - 1; i++){
            new_node->children.push_back(node->children[mid + 1]);
            new_node->keys.push_back(node->keys[mid + 1]);
            removeElem(node->children, mid + 1);
            removeElem(node->keys, mid + 1);
        }
        new_node->children.push_back(node->children.back());
        new_node->parent = node->parent;
        removeElem(node->children, mid + 1);
        removeElem(node->keys, mid);
        int index = searchPos(node->keys, insertKey);
        insertElem(node->parent->keys, index, insertKey);
        insertElem(node->parent->children, index + 1, new_node);
        overFlow(node->parent);
    }
	vector<T> new_keys;
	vector<Node<T>*> new_children;
	for(int i = 0; i <= mid + 1; i++){
		new_keys.push_back(node->keys[i]);
		new_children.push_back(node->children[i]);
	}
}

template<typename T>
bool Tree<T>::recursive_remove(Node<T>* node, T key){
    // function to delete a key in one node and ancestor nodes
    // note that input node has parent
    if(node->keys.size() == 0){
        downFlow(node);
        recursive_remove(node, key);
    }
    else{
        for(int i = 0; i < node->keys.size(); i++){
            if(node->keys[i] == key){
                removeElem(node->keys, i);
                mergeNode(node->children[i], node->children[i + 1]);
                removeElem(node->children, i + 1);
                return true;
            }
        }
        if(node->parent)    recursive_remove(node->parent, key);
        else return true;
    }
}

template <typename T>
void Tree<T>::downFlow(Node<T>* emptyNode){
    if(!emptyNode->parent) return;
    if(emptyNode->keys.size() > 0) return;
    int rank = 0;
    while(emptyNode->parent->children[rank] != emptyNode) rank++;
    // get the index of empty node in this parent's children
    if(rank == 0){
        Node<T>* rs = emptyNode->parent->children[1];
        if(emptyNode->isLeaf){
            if(rs->keys.size() > 1){
                emptyNode->keys.push_back(emptyNode->parent->keys[0]);
                emptyNode->parent->keys[0] = rs->keys[0];
                removeElem(rs->keys, 0);
            }
            else{
                removeElem(emptyNode->parent->children, 0);
                removeElem(emptyNode->keys, 0);
                emptyNode = rs;
                downFlow(emptyNode->parent);
            }
        }
        else{
            if(rs->keys.size() > 1){
                emptyNode->parent->keys[0] = rs->keys[1];
                emptyNode->keys.push_back(rs->keys[0]);
                emptyNode->children[1] = rs->children[1];
                removeElem(rs->children, 1);
                removeElem(rs->keys, 0);
            }
            else{
                mergeNode(emptyNode->children[0], rs->children[0]);
                rs->children[0] = emptyNode->children[0];
                emptyNode->children[0].parent = rs;
                emptyNode = rs;
                removeElem(emptyNode->parent->children, 0);
                removeElem(emptyNode->parent->keys, 0);
                overFlow(emptyNode->children[0]);
            }
        }
    }

}

template <typename T>
bool Tree<T>::remove(const T& key) {
	pair<Node<T>*, int> target = search(key);
	Node<T>* node = target.first;
	int index = target.second;
	if (!node) return false;
    // case 0: node is the root (tree with height 1)
    if (node == _root){
        removeElem(node->keys, index);
        return true;
    }
    // case 1: index != 0
    if (index != 0){
        removeElem(node->keys, index);
        return true;
    }
    if (index == 0) {
        if (node == node->parent->children[0]) {
            // case 2: index == 0, node is the first child of parent;
            // and node has more than one key
            removeElem(node->keys, 0);
            recursive_remove(node, key);
        }
        if (node != node->parent->children[0]) {
            // case 3: index == 0, node isn't the first child of parent;
            if(node->keys.size() > 1){
                int rank = searchPos(node->parent->keys, key);
                node->parent->keys[rank] = node->keys[1];
                removeElem(node->keys, 0);
            }
            else{
                int rank = searchPos(node->parent->keys, key);
                removeElem(node->parent->keys, rank);
                removeElem(node->parent->children, rank + 1);
                downFlow(node->parent);
                delete(node);
            }
        }
    }
}

template <typename T>
void Tree<T>::underFlow(Node<T> * node) {
	if((_order + 1) / 2 <= node->children.size()) return;
	Node<T>* pNode = node->parent;
	if(!pNode){
		if((node->keys.size()==0)&&(node->children[0])){
			// case that the root (node) has no key already but has a sole child
			// which is not empty
			_root = node->children[0];
			_root->parent = NULL;
			node->children[0] = NULL;
			delete node;
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
		if((_order + 1) / 2 < ls->children.size()){
			insertElem(node->keys, 0, pNode->keys[rank - 1]);
			pNode->keys[rank - 1] = ls->keys[ls->keys.size() - 1];
			removeElem(ls->keys, ls->keys.size() - 1);
			insertElem(node->children, 0, ls->children[ls->children.size() - 1]);
			removeElem(ls->children, ls->children.size() - 1);
			if(!node->isLeaf) node->children[0]->parent = node;
			return;
		}
	}
	// case 2: borrow a key from the right subling
	if(pNode->children.size() - 1 > rank) {
        // if node is not the last child of pNode
        // then node certainly has a right subling
        Node<T> *rs = pNode->children[rank + 1];
        if ((_order + 1) / 2 < rs->children.size()) {
            node->keys.push_back(pNode->keys[rank]);
            pNode->keys[rank] = rs->keys[0];
            removeElem(rs->keys, 0);
            node->children.push_back(rs->children[0]);
            removeElem(rs->children, 0);
            if(node->children[node->children.size() - 1]) {
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
        ls->children.push_back(node->children[0]);
        if(ls->children[ls->children.size() - 1]){
            ls->children.back()->parent = ls;
        }
        while(!node->keys.empty()){
            ls->keys.push_back(node->keys[0]);
            removeElem(node->keys, 0);
            ls->children.push_back(node->children[0]);
            removeElem(node->children, 0);
            if(ls->children[ls->children.size() - 1]){
                ls->children[ls->children.size() - 1]->parent = ls;
            }
        }
        delete(node);
    }
    else{
        // merge node with its right subling
        Node<T>* rs = pNode->children[rank + 1];
        insertElem(rs->keys, 0, pNode->keys[rank]);
        removeElem(pNode->keys, rank);
        removeElem(pNode->children, rank);
        insertElem(rs->children, 0, node->children.back());
        removeElem(node->children, node->children.size() - 1);
        if(rs->children[0]){
            rs->children[0]->parent = rs;
        }
        while(!node->keys.empty()){
            insertElem(rs->keys, 0, node->keys.back());
            removeElem(node->keys, node->keys.size() - 1);
            insertElem(rs->children, 0, node->children.back());
            removeElem(node->children, node->children.size() - 1);
            if(rs->children[0]){
                rs->children[0]->parent = rs;
            }
        }
        delete(node);
    }
    underFlow(pNode);
    return;
}

int main() {
    Tree<int> tree;
    cout << "dadaa" << endl;
    cout << "dadaa" << endl;
    tree.insert(2, "2");
    cout << "dadaa" << endl;
    tree.printTree();
    return 0;
}