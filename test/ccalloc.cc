#include "../util/util.h"

#include <memory>
#include <cassert>
#include <thread>
#include <chrono>
#include <exception>

namespace {
template <typename T>
struct StackNode_ {
	T data;
	StackNode_* prev;
};

/** T is the object to store in the stack, Alloc is the allocator to use */
template <class T, class Alloc = std::allocator<T> >
class StackAlloc {
public:
	typedef StackNode_<T> Node;
	typedef typename Alloc::template rebind<Node>::other allocator;

	/** Default constructor */
	StackAlloc() { head_ = 0; }
	/** Default destructor */
	~StackAlloc() { clear(); }

	/** Returns true if the stack is empty */
	bool empty() { return (head_ == 0); }

	/** Deallocate all elements and empty the stack */
	void clear() {
		Node* curr = head_;
		while (curr != 0) {
			Node* tmp = curr->prev;
			allocator_.destroy(curr);
			allocator_.deallocate(curr, 1);
			curr = tmp;
		}
		head_ = 0;
	}

	/** Put an element on the top of the stack */
	void push(T element) {
		Node* newNode = allocator_.allocate(1);	// 分配一个节点内存空间
		allocator_.construct(newNode, Node());	// 构造这个节点数据
		newNode->data = element;	// 设置该节点数据
		newNode->prev = head_;	// 设置前向节点
		head_ = newNode;	// 将头结点移至当前新增节点
	}

	/** Remove and return the topmost element on the stack */
	T pop() {
		T result = head_->data;		// 头部节点数据
		Node* tmp = head_->prev;	// 保存前向节点指针
		allocator_.destroy(head_);	// 调用析构函数
		allocator_.deallocate(head_, 1);	// 归还内存空间
		head_ = tmp;	// 将当前节点退回到前一个节点
		return result;	// 返回销毁节点数据
	}

	/** Return the topmost element */
	T top() { return (head_->data); }	// 获取栈顶元素数据

private:
	allocator allocator_;
	Node* head_;
};

}

#define ELEMS 40000
#define REPS 50

namespace test {

static safe::SyncEvent ev;
static size_t kDone = 0;

static void stdworker() {
	//StackAlloc<int, std::allocator<int> > stackDefault;
	std::vector<std::string, std::allocator<std::string>> stackPool;
	for (int j = 0; j < REPS; j++) {
		assert(stackPool.empty());
		for (int i = 0; i < ELEMS / 4; i++) {
			// Unroll to time the actual code and not the loop
			stackPool.push_back("allen");
			stackPool.push_back("allen");
			stackPool.push_back("allen");
			stackPool.push_back("allen");
		}
		for (int i = 0; i < ELEMS / 4; i++) {
			// Unroll to time the actual code and not the loop
			stackPool.pop_back();
			stackPool.pop_back();
			stackPool.pop_back();
			stackPool.pop_back();
		}
	}

	++kDone;
	ev.signal();
}

static void customworker() {
	COUT << "customworker begin...";
	StackAlloc<int, ccalloc::MemoryPool<int> > stackPool;
	//std::vector<std::string, ccalloc::MemoryPool<std::string>> stackPool;
	for (int j = 0; j < REPS; j++) {
		assert(stackPool.empty());
		for (int i = 0; i < ELEMS / 4; i++) {
			// Unroll to time the actual code and not the loop
			stackPool.push(i);
			stackPool.push(i);
			stackPool.push(i);
			stackPool.push(i);
		}
		for (int i = 0; i < ELEMS / 4; i++) {
			// Unroll to time the actual code and not the loop
			stackPool.pop();
			stackPool.pop();
			stackPool.pop();
			stackPool.pop();
		}
	}
	COUT << "customworker done...";

	++kDone;
	ev.signal();
}

DEF_test(test_ccalloc) {
	sys::timer t;
	DEF_case(default_allocator) {
		StackAlloc<int, std::allocator<int> > stackDefault;
		for (int j = 0; j < REPS; j++) {
			assert(stackDefault.empty());
			for (int i = 0; i < ELEMS / 4; i++) {
				// Unroll to time the actual code and not the loop
				stackDefault.push(i);
				stackDefault.push(i);
				stackDefault.push(i);
				stackDefault.push(i);
			}
			for (int i = 0; i < ELEMS / 4; i++) {
				// Unroll to time the actual code and not the loop
				stackDefault.pop();
				stackDefault.pop();
				stackDefault.pop();
				stackDefault.pop();
			}
		}
	}
	COUT << "default_allocator costs time: " << t.ms();

	t.restart();
	DEF_case(mempool_allocator) {
		StackAlloc<int, ccalloc::MemoryPool<int> > stackPool;
		for (int j = 0; j < REPS; j++) {
			assert(stackPool.empty());
			for (int i = 0; i < ELEMS / 4; i++) {
				// Unroll to time the actual code and not the loop
				stackPool.push(i);
				stackPool.push(i);
				stackPool.push(i);
				stackPool.push(i);
			}
			for (int i = 0; i < ELEMS / 4; i++) {
				// Unroll to time the actual code and not the loop
				stackPool.pop();
				stackPool.pop();
				stackPool.pop();
				stackPool.pop();
			}
		}
	}
	COUT << "mem_allocator costs time: " << t.ms();
}

// 下面这种会崩溃,不能用于复杂的数据结构
DEF_test(test_vector_mempool) {
	try{
		std::vector<std::string, ccalloc::MemoryPool<std::string> > stackPool;
		for (int j = 0; j < REPS; j++) {
			assert(stackPool.empty());
			for (int i = 0; i < ELEMS / 4; i++) {
				// Unroll to time the actual code and not the loop
				stackPool.push_back("allen");
				stackPool.push_back("allen");
				stackPool.push_back("allen");
				stackPool.push_back("allen");
			}
			for (int i = 0; i < ELEMS / 4; i++) {
				// Unroll to time the actual code and not the loop
				stackPool.pop_back();
				stackPool.pop_back();
				stackPool.pop_back();
				stackPool.pop_back();
			}
		}
	}catch (std::bad_alloc &e){
		std::cerr << e.what() << std::endl;
	}
}

DEF_test(test_thread_ccalloc) {
	DEF_case(default_allocator) {
		using ThreadPtr = std::shared_ptr<std::thread>;
		using ThreadPtrVector = std::vector<ThreadPtr>;

		kDone = 0;
		sys::timer t;
		ThreadPtrVector v;
		for (size_t i = 0; i < 4; ++i) {
			ThreadPtr t(new std::thread(stdworker));
			v.push_back(t);
		}
		COUT << "[1] vector size: " << v.size();

		do {
			ev.wait();
		} while (kDone < 4);

		COUT << "[2] vector size: " << v.size();

		COUT << "default_allocator costs time: " << t.ms();

		for (auto &x : v) {
			if(x)
				x->join();
			else
				CERR << "null";
		}

		COUT << "[3] vector size: " << v.size();
	}
	
	COUT << "----------------------------";

	DEF_case(mem_allocator) {
		using ThreadPtr = std::shared_ptr<safe::Thread>;
		using ThreadPtrVector = std::vector<ThreadPtr>;

		kDone = 0;
		sys::timer t;
		ThreadPtrVector v;
		for (size_t i = 0; i < 4; ++i) {
			ThreadPtr t(new safe::Thread(std::bind(customworker)));
			v.push_back(t);
			t->start();
		}
		COUT << "[1] vector size: " << v.size();

		do {
			ev.wait();
		} while (kDone < 4);

		COUT << "[2] vector size: " << v.size();

		COUT << "mem_allocator costs time: " << t.ms();

		for (auto &x : v) {
			if (x)
				x->join();
			else
				CERR << "null";
		}

		COUT << "[3] vector size: " << v.size();
	}
}

}//namespace test