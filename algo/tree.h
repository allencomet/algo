#pragma once

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

	Value* search(const Key &k) {
		return search_internal(_root,k);
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
	void insert_internal(Node *node, const Key &k, const Value &v) {
		if (NULL == node) {
			++_count;
			node = new Node(k, v);//有问题：实际上层的指针并不会改变NULL的性质，因为要带出去必须使用二级指针
			return;
		}

		if (node->key == k) {
			node->value = v;
		} else if (k < node->key) {
			insert_internal(node->lchild, k, v);
		} else {
			insert_internal(node->rchild, k, v);
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