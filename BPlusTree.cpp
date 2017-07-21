#include<iostream>
#include<vector>

using namespace std;

const int ORDER = 4;  // minnum order of b+ tree
const int MINNUM_KEY = ORDER - 1; // minnum of key number for a node
const int MAXNUM_KEY = 2 * ORDER - 1; // maxnum of key number for a node
const int MINNUM_CHILD = MINNUM_KEY + 1; // minnum of children for a node
const int MAXNUM_CHILD = MAXNUM_KEY + 1; // maxnum of children for a node

template<typename keyclass>
int searchPos(vector<keyclass> keys, keyclass key) {
	int size = keys.size();
	for (int i = 0; i < size; i++) {
		if (key < keys[i]) return i;
	}
	return size;
}

template <typename keyclass>
class Node{
public:
	Node(){
		parent = nullptr;
		vector<keyclass> keys;
		vector<Node<keyclass>*> children;
	}
	Node(keyclass key, Node<keyclass>* lc = NULL, Node<keyclass>* rc = NULL){
		vector<keyclass> keys;
		vector<Node<keyclass>*> children;
		parent = NULL;
		keys.insert(0, key);
		children.insert(0, lc);
		children.insert(1, rc);
		if(lc) lc->parent = this;
		if(rc) rc->parent = this;
	}
	bool leaf(){return isLeaf;}
	vector<keyclass> getKeys(){return keys;}
	vector<Node<keyclass>*> getChildren(){return children;}
	int getKeyNum(){return keys.size();}
	Node<keyclass>* getPar(){return parent;}
private:
	bool isLeaf;
	vector<keyclass> keys;
	vector<Node<keyclass>*> children;
	Node<keyclass>* parent;
};

template <typename keyclass>
class Tree{
protected:
	int _size;
	int _order;
	Node<keyclass>* _root;
	Node<keyclass>* _hook;
	void overFlow(Node<keyclass>*);
	void underFlow(Node<keyclass>*);
public:
	Tree(int order = order): _order(order), _size(0)
	{
		_root = new Node<keyclass>();
	}
	~Tree(){if(root) delete(_root);}
	int const order(){return _order;}
	int const size(){return _size;}
	Node<keyclass>* root(){return _root;}
	bool empty() const {return !_root;}
	pair<Node<keyclass>*, int> search(const keyclass& key);
	bool insert(const keyclass& key);
	bool remove(const keyclass& key);
};

template <typename keyclass>
pair<Node<keyclass>*, int> Tree<keyclass>::search(const keyclass &key) {
	Node<keyclass>* node = _root;
	_hook = NULL;
	while(node){
		if(node->leaf()){
			vector<keyclass> keys = node->getKeys();
			int keyNum = keys.size();
			for(int i = 0; i < keyNum; i++) {
				if (keys[i] == key) return pair(node, i);
			}
		}
		else{
			vector<keyclass> keys = node->getKeys();
			int keyNum = keys.size();
			int childIndex = 0;
			while(childIndex <= keyNum){
				if(key < keys[childIndex]){
					node = node->getChildren()[childIndex];
					break;
				}
				if(childIndex == keyNum){
					node = node->getChildren()[childIndex];
					break;
				}
				childIndex++;
			}
		}
	}
	return NULL;
}

template <typename keyclass>
bool Tree<keyclass>::insert(const keyclass &key){
	Node<keyclass>* node = this->search(key).first;
	if(node) return false;
}

template <typename keyclass>
void Tree<keyclass>::overFlow(Node<keyclass>* node) {
	if(_order >= node->getChildren().size()) return;
	int mid = _order / 2;
	keyclass insertKey = node->getKeys()[mid]
	Node<keyclass>* new_node = new Node<keyclass>();
	for(int i = 0; i < _order - mid - 1; i++){
		new_node->getChildren().insert(i, node->getChildren().remove(mid + i));
		new_node->getKeys().insert(i, node->getKeys().remove(mid + 1));
	}
	new_node->getChildren()[_order - mid - 1] = node->getChildren().remove(s + 1);
	if(!new_node->getChildren().empty()){
		int childNum = new_node->getKeyNum() + 1;
		for(int i = 0; i < childNum; i++) new_node->getChildren()[i]->parent = new_node;
		// make child of new node's parent the new node
	}
	Node<keyclass>* pNode = node->getPar();
	if(!pNode) {
		_root = pNode = new Node<keyclass>();
		pNode->getChildren()[0] = new_node;
	}
	int rank = 0;
	while(rank < pNode->getKeyNum()){
		if(insertKey < pNode->getKeys()[rank]) {
			pNode->getKeys().insert(rank, insertKey);
			node->getKeys().remove(mid);
			break;
		}
		rank++;
	}
	pNode->getChildren().insert(rank + 1, new_node);
	new_node->getPar() = pNode;
	overFlow(pNode);
}

template <typename keyclass>
bool Tree<keyclass>::remove(const keyclass& key) {
	Node<keyclass>* node = search(key).first;
	if (!node) return false;
	int rank = searchPos(node->getKeys(), key);
	if (!node->isLeaf()) {
		Node<keyclass>* child = node->getChildren()[rank + 1];
		while (!child->isLeaf()) child = child->getChildren[0];
		node->getKeys[rank] = child->getKeys[0];
		node = child;
	}
	
}
