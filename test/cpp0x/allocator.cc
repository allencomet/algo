#include "../../util/util.h"

#include<thread>
#include<chrono>

namespace test {

DEF_test(mem_std_allocator) {
	/*int arr[] = { 0,1,2,3,4,5 };
	mem::malloc_allocator<int> t;
	std::vector<int, mem::malloc_allocator<int>> iv(arr, arr + 6);
	for (size_t i = 0; i < iv.size(); ++i){
		std::cout << iv[i] << " ";
	}
	std::cout << std::endl;*/

	std::vector<std::string> v(1 << 25,"allen");
}

DEF_test(mem_own_allocator) {
	std::vector<std::string, mem::stdmem::malloc_allocator<std::string>> v(1 << 25,"allen");
}

DEF_test(jemem_allocator) {
	std::vector<std::string, mem::jemem::malloc_allocator<std::string>> v(1 << 25,"allen");
}

DEF_test(jemalloc) {
	int *piyo = (int *)je_malloc(sizeof(int));
	if (nullptr == piyo) {
		CERR << "je_malloc failed";
		return;
	}

	printf("piyo = %p, %d.\n", piyo, *piyo);
	je_free(piyo);
	piyo = nullptr;

	piyo = (int *)je_calloc(2, sizeof(int));
	if (nullptr == piyo) {
		CERR << "je_calloc failed";
		return;
	}
	printf("piyo = %p, %d.\n", piyo, *piyo);

	piyo = (int *)je_realloc(piyo, sizeof(int));
	if (nullptr == piyo) {
		CERR << "je_realloc failed";
		return;
	}
	printf("piyo = %p, %d.\n", piyo, *piyo);
	je_free(piyo);
	piyo = nullptr;
}

static void jeworker() {
	std::cout << "ID:" << std::this_thread::get_id() << std::endl;
	for (int i=0; i<5; ++i){
		std::vector<std::string, mem::jemem::malloc_allocator<std::string>> v(1 << 25, "allen");
	}
}

DEF_test(test_multithread_jemalloc) {
	//获取CPU的核数
	std::cout << "CPU: " << std::thread::hardware_concurrency() << std::endl;
	using ThreadPtr = std::shared_ptr<std::thread>;
	std::vector<ThreadPtr, mem::jemem::malloc_allocator<ThreadPtr>> v;
	for (int i=0;i<10;++i){
		ThreadPtr t = std::make_shared<std::thread>(jeworker);
		v.push_back(t);
	}

	for (auto &x : v) {
		x->join();
	}
}

static void worker() {
	std::cout << "ID:" << std::this_thread::get_id() << std::endl;
	for (int i = 0; i < 5; ++i) {
		std::vector<std::string> v(1 << 25, "allen");
	}
}

DEF_test(test_multithread_stdmalloc) {
	//获取CPU的核数
	std::cout << "CPU: " << std::thread::hardware_concurrency() << std::endl;
	using ThreadPtr = std::shared_ptr<std::thread>;
	std::vector<ThreadPtr> v;
	for (int i = 0; i < 10; ++i) {
		ThreadPtr t = std::make_shared<std::thread>(worker);
		v.push_back(t);
	}

	for (auto &x : v) {
		x->join();
	}
}

}//namespace test


/************************************************************************/
/* 


当我们使用new表达式，来调用拷贝构造函数时实际上时伴随着空间的分配的。那么Allocator是怎么做到的呢？
实际上它是调用了C++的一个内置的操作符:

void *operator new(size_t); // allocate an object
void *operator new[](size_t); // allocate an array
new (place_address) type
new (place_address) type (initializer-list)

这些重载操作符函数可以进行内存的分配以及在指定的内存空间进行对象的构造。需要注意的是它们并非new表达式，
new表达式是不可以被重载的，实际上new表达式底层也就是调用了这些重载函数的。前两个用内存的分配，后两个则
用于对象的构造。Alloctor<T>类的construct方法底层实现实际上就是一句调用而已：

new (first_free) T(const T& t);

我们再来看一下reallocate函数的实现：

template <class T> 
void Vector<T>::reallocate() {
	// compute size of current array and allocate space for twice as many elements
	std::ptrdiff_t size = first_free - elements;	// 计算当前内存大小
	std::ptrdiff_t newcapacity = 2 * max(size, 1);	// 计算重新申请的大小为当前大小的两倍

	// allocate space to hold new capacity number of elements of type T
	T* newelements = alloc.allocate(newcapacity);	// 分配两倍两倍当前大小的内存空间

	// construct copies of the existing elements in the new space
	uninitialized_copy(elements, first_free, newelements);	// 将原来内存区域的数据赋值到新的地址空间

	// destroy the old elements in reverse order
	for (T *p = first_free; p != elements;  empty  )
		alloc.destroy(--p);	// 对原来内存区域的数据进行析构操作，但不是将内存归还操作系统

	// deallocate cannot be called on a 0 pointer  
	if (elements){
		// return the memory that held the elements
		alloc.deallocate(elements, end - elements);		// 将原来的内存区域空间归还给操作系统
	}

	// make our data structure point to the new elements  
	elements = newelements;	// 将新的内存地址设置到elements
	first_free = elements + size;	// 第一个可用地址
	end = elements + newcapacity;	// 结束地址
}

Alloctor<T>类的allocate成员函数的使用，它的作用是向系统申请指定个数的长度为sizeof(T)的连续空间，其底层实现是：

return operator new[](newcapacity * sizeof(T));

关于Alloctor<T>类的deallocate成员函数的分析，它有两个参数，第一个指向线性表的首地址，第二个参数指明要内存回收
的对象的个数，注意，这里只进行内存回收，而不会进行对象的销毁，其底层使用的是delete重载函数，同new重载函数一样，
它也不是我们所熟知的delete表达式，而delete表达式其底层则是调用了delete重载函数来释放内存的，先来看delete有哪些
重载函数：

void *operator delete(void*); // free an object
void *operator delete[](void*); // free an array



如何实现一个自己的allocator

根据Allocator requirements我们需要提供一些typedefs：
template <typename T>
class CHxAllocator
{
public:
// typedefs...
typedef T                   value_type;
typedef value_type*         pointer;
typedef value_type&         reference;
typedef value_type const*   const_pointer;
typedef value_type const&   const_reference;
typedef size_t              size_type;
typedef ptrdiff_t           difference_type;

// rebind...
template <typename _other> struct rebind { typedef CHxAllocator<_other> other; };
};

这里有一个比较不太容易理解的东西：rebind。C++标准里这么描述rebind的：

The member class template rebind in the table above is effectively a typedef template: 
if the name Allocator is bound to SomeAllocator<T>, then

Allocator::rebind<U>::other is the same type as SomeAllocator<U>.

啥意思？可以用一个简单的例子来说明下：

学校都学过数据结构，比方说栈、单向列表、树。我们就拿栈和列表来对比，看看有什么大不一样的地方。
撇开数据结构上的差异，从allocator的角度来看，我们可以发现：堆栈是存贮元素本身的，但是列表实际
上不是直接存储元素本身的。要维护一个列表，我们至少还需要一个所谓的next的指针。因此，虽然是一个
保存int的列表list<int>，但是列表存储的对象并不是int本身，而是一个数据结构，它保存了int并且还包
含指向前后元素的指针。那么，list<int, allocator<int>>如何知道分配这个内部数据结构呢？
毕竟allocator<int>只知道分配int类型的空间。这就是rebind要解决的问题。
通过allocator<int>::rebind<_Node>()你就可以创建出用于分配_Node类型空间的分配器了。

接下来要提供其他的接口。根据The default allocator的描述，我们要提供如下一些接口：

返回val的地址
pointer address(reference val) constconst_pointer address(const_reference val) const	

分配空间。类似malloc。pHint可以无视，主要是给类库使用，用于提高性能。
pointer allocate(size_type cnt, CHxAllocator<void>::const_pointer pHint = 0)	

释放空间，类似free。
void deallocate(pointer p, size_type n)	

可分配的最大数量。
size_type max_size() const throw()	

在地址p所指向的空间，使用val进行填充。需要使用到palcement new，以便保证调用到构造函数。
void construct(pointer p, const_reference val)	

析构p指向的内存块中内容。一般通过显示调研析构函数来执行。
void destroy(pointer p)	

各种构造函数和析构函数
allocator() throw ()
allocator(const_reference) throw ()
template <typename _other> allocator(CHxAllocator <_other> const&) throw()
~CHxAllocator() throw()

如何实现上面这些函数，你只要照抄标准库中的实现就可以了。如果你想要用c的malloc和free来实现，也可以这么写：

pointer allocate(size_type cnt, CHxAllocator<void>::const_pointer pHint = 0)
{
	UNREFERENCED_PARAMETER(pHint);

	if (cnt <= 0){
		return 0;
	}

	void* pMem = nullptr;
	if (max_size() < cnt || (pMem = malloc(cnt * sizeof(value_type))) == NULL){
		throw std::bad_alloc(0);
	}
	return static_cast <pointer>(pMem);
}

void deallocate(pointer p, size_type){
	free(p);
}

void construct(pointer p, const_reference val){
	::new ((void *)p) T(val);
}

void destroy(pointer p){
	p->~T();
}

基本上，我们就简单实现了一个自己的allocator。另外，除了这些最主要的接口函数，你还需要实现比较操作符==和!=，
但是这些函根据标准文档，都直接返回true和false。

重写allocator的主要目的是为了提高性能。那怎样才能提高性能呢？直接使用Windows的HeapXXXX堆内存API？
其实，你自己用一下就会发现，性能提升并不明显。因为通过new，再通过malloc，最后通过HeapAlloc不比直接调用HeapAlloc多几句话。
如何实现一个高性能的allocator，需要借助memory pool的想法。另外，侯捷的stl源码剖析里分析了SGI STL利用类似想法实现的一个alloc


#ifndef _CGH_ALLOC
#define _CGH_ALLOC

#include <new>
#include <cstddef>
#include <cstdlib>
#include <climits>
#include <iostream>

namespace CGH
{
template<class T>
inline T* _allocate(ptrdiff_t size, T*)
{
set_new_handler(0);
T* tmp = (T*)(::operator new((size_t)(size * sizeof(T)))); // size：待分配的元素量，sizeof(T)每个元素的大小
if (tmp == 0)
{
cerr << "out of memory" << endl;
exit(1);
}
return tmp;
}

template<class T>
inline void _deallocate(T* buffer)
{
::operator delete(buffer);
}

template<class T1, class  T2>
inline void _construct(T1* p, const T2& value)
{
new(p)T1(value); // 调用placement new，在指定的内存位置p处初始化T1对象，初始化T1对象时调用T1的复制构造函数
}

template<class T>
inline void _destroy(T* ptr)
{
ptr->~T();
}

template<class T>
class allocator
{
public:
typedef T           value_type;
typedef T*          pointer;
typedef const T*    const_pointer;
typedef T&          reference;
typedef const T&    const_reference;
typedef size_t      size_type;
typedef ptrdiff_t   difference_type;

template<class U>
struct rebind
{
typedef allocator<U> other;
};

pointer allocate(size_type n, const void* hint = 0)
{
return _allocate((difference_type)n, (pointer)0);
}

void deallocate(pointer p, size_type n)
{
_deallocate(p);
}

void construct(pointer p, const T& value)
{
_construct(p, value);
}

void destroy(pointer p)
{
_destroy(p);
}

pointer address(reference x)
{
return (pointer)&x;
}

const_pointer const_address(const_reference x)
{
return (const_pointer)&x;
}

size_type max_size() const
{
return size_type(UINT_MAX / sizeof(T));
}
};
}

#endif

*/
/************************************************************************/