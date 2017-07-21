#include<iostream>
#include<vector>

using namespace std;

class Node {
public:
	int sizeOfKey();
	int numOfChild();
	Node* getChild(int childIndex);
	int getKey(int keyIndex);
	bool isLeaf();
	vector<int> getKeys();
private:
	vector<Node*> Children;
	int keyNum;
	bool isLeaf();
	vector<int> Keys;
};

class Tree {
public:
	Node* getRoot();
	int getDepth();
private:
	Node* root;
	int depth;
};

pair<Node*, int> search(Node* node, int key) {
	int i = 1;
	vector<int> keys = node -> getKeys();
	while ((i <= node->sizeOfKey) && (key > keys[i])) {
		i += 1;
	}
	if ((i <= node->sizeOfKey()) && (key == keys[i])) {
		pair<Node*, int> res(node, i);
		return res;
	}
	else if (node->isLeaf()) {
		pair<Node*, int> res(nullptr, 0);
		return res;
	}
	else {
		node = node->getChild(i);
		return search(node, key);
	}
}

