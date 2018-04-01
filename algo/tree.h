#pragma once

#include <deque>

namespace tree {




//二分法查找原理
//在[l,r]中查找，先和mid=(l+r)/2位置比较，如果等于则直接返回，小于继续在左边找，大于在右边继续查找
template<typename T>
bool BinarySearch(T arr[], int n, T target) {
	int mid;
	int l = 0, r = n - 1;
	for (; l <= r;) {
		mid = l + (r - l) / 2;//mid = (l+r)/2可能会越界
		if (arr[mid] == target) {
			return true;
		}
		else if (target < arr[mid]) {//继续在左边查找
			r = mid - 1;
		}
		else {
			l = mid + 1;
		}
	}

	return false;
}

//在[l,r]区域查找目标
template<typename T>
bool BinarySearchRecursive_child(T arr[], int l,int r, T target) {
	if (l == r) {
		if (arr[l] == target) return true;
		else return false;
	} 

	int mid = l + (r - l) / 2;
	if (arr[mid] == target) {
		return true;
	} else if (target < arr[mid]) {
		BinarySearchRecursive_child(arr, l, mid - 1, target);
	} else {
		BinarySearchRecursive_child(arr, mid+1, r, target);
	}
}


template<typename T>
bool BinarySearchRecursive(T arr[], int n, T target) {
	return BinarySearchRecursive_child(arr, 0, n - 1, target);
}


template<typename Key,typename Value>
class BST {
public:
	BST() {
		_root = NULL;
		_count = _capacity = 0;
	}
	~BST() {
	
	}

	int size() const {
		return _count;
	}

	int capacity() const {
		return _capacity;
	}

	bool empty() const {
		return _count == 0;
	}

	void insert(const Key &k, const Value &v) {
		//insert_internal(_root, k, v);
		_root = insert(_root, k, v);
	}

	//这是错误的写法
	void insert_norecursive_error1(const Key &k, const Value &v) {
		Node *node = _root;
		while (node) {
			if (k == node->key) {
				node->value = v;
				return;
			}else if (k < node->key) {
				node = node->lchild;
			}else {
				node = node->rchild;
			}
		}

		node = new Node(k, v);
		++_count;
	}

	void insert_norecursive_error2(const Key &k, const Value &v) {
		if (NULL != _root) {
			Node *node = _root;
			while (node) {
				if (k == node->key) {
					node->value = v;
					return;
				} else if (k < node->key) {
					node = node->lchild;
				} else {
					node = node->rchild;
				}
			}
			node = new Node(k, v);
			++_count;
		} else {
			_root = new Node(k, v);
			++_count;
		}
	}

	void insert_norecursive(const Key &k, const Value &v) {
		if (NULL != _root) {
			Node *node = _root;
			for (;;) {
				if (k == node->key) {
					node->value = v;
					break;
				}else if (k < node->key) {
					if (NULL == node->lchild) {
						node->lchild = new Node(k, v);
						++_count;
						break;
					} else {
						node = node->lchild;
					}
				}else {
					if (NULL == node->rchild) {
						node->rchild = new Node(k, v);
						++_count;
						break;
					} else {
						node = node->rchild;
					}
				}
			}
		}else {
			_root = new Node(k, v);
			++_count;
		}
	}

	Value* search(const Key &k) {
		return search_internal(_root,k);
	}

	void preOrder() {
		COUT << "preOrder: ";
		preOrder_recursive(_root);
	}

	void inOrder() {
		COUT << "inOrder: ";
		inOrder_recursive(_root);
	}

	void postOrder() {
		COUT << "postOrder: ";
		postOrder_recursive(_root);
	}

	void levelOrder() {
		COUT << "levelOrder: ";
		std::deque<Node *> q;
		Node* node = NULL;
		q.push_back(_root);
		while (!q.empty()) {
			node = q.front();
			if (NULL != node) {
				COUT << "key: " << node->key << ",value: " << node->value;
				if (NULL != node->lchild) q.push_back(node->lchild);
				if (NULL != node->rchild) q.push_back(node->rchild);
			}
			q.pop_front();
		}
	}

	std::pair<Key,Value> minimum() {
		Node *node = _root;
		for (;;) {
			if(NULL == node) break;
			if (node->lchild)
				node = node->lchild;
			else
				return std::make_pair(node->key, node->value);
		}
	}

	std::pair<Key,Value> maximum() {
		Node *node = _root;
		for (;;) {
			if (NULL == node) break;
			if (node->rchild)
				node = node->rchild;
			else
				return std::make_pair(node->key, node->value);
		}
	}

	void hubbardDel_error(const Key &k) {
		//原理：将右子树的最小值移动过来 || 将左子树的最小值移动过来
		Node *node = _root;
		while (node) {
			if (node->key == k) {
				Node *temp = extractMinimum_error();
				delete node;
				node = temp;	//还得把上一层的
				--_count;
				break;
			}else if (k < node->key) {
				node = node->lchild;
			}else {
				node = node->rchild;
			}
		}
	}

	//错误版本1
	void delMinimum_error1() {
		Node *node = _root;
		while (node) {
			if (node->lchild) {
				node = node->lchild;
			} else {
				delete node;
				node = NULL;//因为没有把父节点的lchild置为当前节点的右节点
				--_count;
				break;
			}
		}
	}

	//错误版本2，即使判断两层那也只能保证最上两层正确
	void delMinimum_error2() {
		Node *node = _root;
		Node *parent = NULL;
		if (NULL == node) return;
		if (node->lchild) {
			parent = node;
			node = node->lchild;
			if (node->lchild) {
				node = node->lchild;
			} else {
				parent->lchild = node->rchild;
				delete node;
				node = NULL;
				--_count;
				return;
			}
		} else {
			delete node;
			node = NULL;
			_root->lchild = NULL;
			--_count;
			return;
		}

		for (;;) {
			if (NULL == node) break;
			if (node->lchild) {
				parent = node;
				node = node->lchild;
				if (node->lchild) {
					node = node->lchild;
				} else {
					parent->lchild = node->rchild;
					delete node;
					node = NULL;
					--_count;
					break;
				}
			} else {
				parent->lchild = node->rchild;
				delete node;
				node = NULL;
				--_count;
				break;
			}
		}
	}

	void delMinRecursive() {
		_root = delMinRecursive_internal(_root);
	}

	//错误版本1
	void delMaximum_error1() {
		Node *node = _root;
		while (node) {
			if (node->rchild) {
				node = node->rchild;
			} else {
				delete node;
				node = NULL;//因为没有把父节点的rchild置为当前节点的左节点
				--_count;
				break;
			}
		}
	}

	//错误版本2
	void delMaximum_error2() {
		Node *node = _root;
		Node *parent = NULL;
		if (NULL == node) return;
		if (node->rchild) {
			parent = node;
			node = node->rchild;
			if (node->rchild) {
				node = node->rchild;
			} else {
				parent->rchild = node->lchild;
				delete node;
				node = NULL;
				--_count;
				return;
			}
		} else {
			delete node;
			node = NULL;
			_root->rchild = NULL;
			--_count;
			return;
		}

		for (;;) {
			if (NULL == node) break;
			if (node->rchild) {
				parent = node;
				node = node->rchild;
				if (node->rchild) {
					node = node->rchild;
				} else {
					parent->rchild = node->lchild;
					delete node;
					node = NULL;
					--_count;
					return;
				}
			} else {
				parent->rchild = node->lchild;
				delete node;
				node = NULL;
				--_count;
				return;
			}
		}
	}

	void delMaxRecursive() {
		_root = delMaxRecursive_internal(_root);
	}

	void del(const Key &k) {
		COUT << "after removing [" << k << "]: ";
		_root = del_internal(_root, k);
	}
private:
	typedef struct Node {
		Node(const Key &k,const Value &v)
			:key(k), value(v),lchild(NULL),rchild(NULL) {
		}

		Key key;
		Value value;
		Node *lchild;
		Node *rchild;
	}Node;

	//错误版本
	void insert_error(Node *node, const Key &k, const Value &v) {
		if (NULL == node) {
			++_count;
			node = new Node(k, v);//有问题：实际上层的指针并不会改变NULL的性质，因为要带出去必须使用二级指针
			return;
		}

		if (node->key == k) {
			node->value = v;
		} else if (k < node->key) {
			insert_error(node->lchild, k, v);
		} else {
			insert_error(node->rchild, k, v);
		}
	}

	Node* insert(Node *node, const Key &k, const Value &v) {
		if (NULL == node) {
			++_count;
			return (node = new Node(k, v));
		}

		if (node->key == k) {
			node->value = v;
		} else if (k < node->key) {
			node->lchild = insert(node->lchild, k, v);
		} else {
			node->rchild = insert(node->rchild, k, v);
		}

		return node;
	}

	void preOrder_recursive(Node *node) {
		if (NULL != node) {
			COUT << "key: " << node->key << ",value: " << node->value;
			preOrder_recursive(node->lchild);
			preOrder_recursive(node->rchild);
		}
	}

	void inOrder_recursive(Node *node) {
		if (NULL != node) {
			inOrder_recursive(node->lchild);
			COUT << "key: " << node->key << ",value: " << node->value;
			inOrder_recursive(node->rchild);
		}
	}

	void postOrder_recursive(Node *node) {
		if (NULL != node) {
			postOrder_recursive(node->lchild);
			postOrder_recursive(node->rchild);
			COUT << "key: " << node->key << ",value: " << node->value;
		}
	}

	Node* delMinRecursive_internal(Node *node) {
		if (NULL == node) return NULL;
		if (NULL != node->lchild) {
			node->lchild = delMinRecursive_internal(node->lchild);
			return node;
		} else {
			Node *rNode = node->rchild;
			delete node;
			node = NULL;
			--_count;
			return rNode;
		}
	}

	Node* delMaxRecursive_internal(Node *node) {
		if (NULL == node) return NULL;
		if (NULL != node->rchild) {
			node->rchild = delMaxRecursive_internal(node->rchild);
			return node;
		} else {
			Node *lNode = node->lchild;
			delete node;
			node = NULL;
			--_count;
			return lNode;
		}
	}

	//错误版本
	Node *extractMinimum_error() {
		Node *node = _root;
		Node *parent = NULL;
		if (NULL == node) return NULL;
		if (node->lchild) {
			parent = node;
			node = node->lchild;
			if (node->lchild) {
				node = node->lchild;
			} else {
				parent->lchild = node->rchild;
				--_count;
				return node;
			}
		} else {
			_root->lchild = NULL;
			--_count;
			return node;
		}

		for (;;) {
			if (NULL == node) break;
			if (node->lchild) {
				parent = node;
				node = node->lchild;
				if (node->lchild) {
					node = node->lchild;
				} else {
					parent->lchild = node->rchild;
					--_count;
					return node;
				}
			} else {
				parent->lchild = node->rchild;
				--_count;
				return node;
			}
		}
	}

	Node *del_internal(Node *node,const Key &k) {
		if (NULL == node) return NULL;

		if (k < node->key) {
			node->lchild = del_internal(node->lchild,k);
			return node;
		} else if (k > node->key){
			node->rchild = del_internal(node->rchild,k);
			return node;
		} else {
			if (node->lchild){
				Node *temp = node->lchild;
				delete node;
				--_count;
				return temp;
			}

			if (node->rchild) {
				Node *temp = node->rchild;
				delete node;
				--_count;
				return temp;
			}

			//左右子树都不为空
			std::pair<Key, Value> min = maximum();//将左子树最大值取出
			Node *successor = new Node(min.first, min.second);

			successor->lchild = delMaxRecursive_internal(node->lchild);//已经再内部_count递减
			successor->rchild = node->rchild;

			delete node;

			return successor;
		}
	}

	Value* search_internal(Node *node,const Key &k) {
		if (NULL == node) return NULL;

		if (node->key == k) {
			return &node->value;
		}else if (k < node->key) {
			return search_internal(node->lchild, k);
		}else{
			return search_internal(node->rchild, k);
		}
	}
	
	Node *_root;//根节点
	int _capacity;
	int _count;

};

}