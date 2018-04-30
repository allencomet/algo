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
	//��ȡCPU�ĺ���
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
	//��ȡCPU�ĺ���
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


������ʹ��new���ʽ�������ÿ������캯��ʱʵ����ʱ�����ſռ�ķ���ġ���ôAllocator����ô�������أ�
ʵ�������ǵ�����C++��һ�����õĲ�����:

void *operator new(size_t); // allocate an object
void *operator new[](size_t); // allocate an array
new (place_address) type
new (place_address) type (initializer-list)

��Щ���ز������������Խ����ڴ�ķ����Լ���ָ�����ڴ�ռ���ж���Ĺ��졣��Ҫע��������ǲ���new���ʽ��
new���ʽ�ǲ����Ա����صģ�ʵ����new���ʽ�ײ�Ҳ���ǵ�������Щ���غ����ġ�ǰ�������ڴ�ķ��䣬��������
���ڶ���Ĺ��졣Alloctor<T>���construct�����ײ�ʵ��ʵ���Ͼ���һ����ö��ѣ�

new (first_free) T(const T& t);

����������һ��reallocate������ʵ�֣�

template <class T> 
void Vector<T>::reallocate() {
	// compute size of current array and allocate space for twice as many elements
	std::ptrdiff_t size = first_free - elements;	// ���㵱ǰ�ڴ��С
	std::ptrdiff_t newcapacity = 2 * max(size, 1);	// ������������Ĵ�СΪ��ǰ��С������

	// allocate space to hold new capacity number of elements of type T
	T* newelements = alloc.allocate(newcapacity);	// ��������������ǰ��С���ڴ�ռ�

	// construct copies of the existing elements in the new space
	uninitialized_copy(elements, first_free, newelements);	// ��ԭ���ڴ���������ݸ�ֵ���µĵ�ַ�ռ�

	// destroy the old elements in reverse order
	for (T *p = first_free; p != elements;  empty  )
		alloc.destroy(--p);	// ��ԭ���ڴ���������ݽ������������������ǽ��ڴ�黹����ϵͳ

	// deallocate cannot be called on a 0 pointer  
	if (elements){
		// return the memory that held the elements
		alloc.deallocate(elements, end - elements);		// ��ԭ�����ڴ�����ռ�黹������ϵͳ
	}

	// make our data structure point to the new elements  
	elements = newelements;	// ���µ��ڴ��ַ���õ�elements
	first_free = elements + size;	// ��һ�����õ�ַ
	end = elements + newcapacity;	// ������ַ
}

Alloctor<T>���allocate��Ա������ʹ�ã�������������ϵͳ����ָ�������ĳ���Ϊsizeof(T)�������ռ䣬��ײ�ʵ���ǣ�

return operator new[](newcapacity * sizeof(T));

����Alloctor<T>���deallocate��Ա�����ķ���������������������һ��ָ�����Ա���׵�ַ���ڶ�������ָ��Ҫ�ڴ����
�Ķ���ĸ�����ע�⣬����ֻ�����ڴ���գ���������ж�������٣���ײ�ʹ�õ���delete���غ�����ͬnew���غ���һ����
��Ҳ������������֪��delete���ʽ����delete���ʽ��ײ����ǵ�����delete���غ������ͷ��ڴ�ģ�������delete����Щ
���غ�����

void *operator delete(void*); // free an object
void *operator delete[](void*); // free an array



���ʵ��һ���Լ���allocator

����Allocator requirements������Ҫ�ṩһЩtypedefs��
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

������һ���Ƚϲ�̫�������Ķ�����rebind��C++��׼����ô����rebind�ģ�

The member class template rebind in the table above is effectively a typedef template: 
if the name Allocator is bound to SomeAllocator<T>, then

Allocator::rebind<U>::other is the same type as SomeAllocator<U>.

ɶ��˼��������һ���򵥵�������˵���£�

ѧУ��ѧ�����ݽṹ���ȷ�˵ջ�������б��������Ǿ���ջ���б����Աȣ�������ʲô��һ���ĵط���
Ʋ�����ݽṹ�ϵĲ��죬��allocator�ĽǶ����������ǿ��Է��֣���ջ�Ǵ���Ԫ�ر���ģ������б�ʵ��
�ϲ���ֱ�Ӵ洢Ԫ�ر���ġ�Ҫά��һ���б��������ٻ���Ҫһ����ν��next��ָ�롣��ˣ���Ȼ��һ��
����int���б�list<int>�������б�洢�Ķ��󲢲���int��������һ�����ݽṹ����������int���һ���
��ָ��ǰ��Ԫ�ص�ָ�롣��ô��list<int, allocator<int>>���֪����������ڲ����ݽṹ�أ�
�Ͼ�allocator<int>ֻ֪������int���͵Ŀռ䡣�����rebindҪ��������⡣
ͨ��allocator<int>::rebind<_Node>()��Ϳ��Դ��������ڷ���_Node���Ϳռ�ķ������ˡ�

������Ҫ�ṩ�����Ľӿڡ�����The default allocator������������Ҫ�ṩ����һЩ�ӿڣ�

����val�ĵ�ַ
pointer address(reference val) constconst_pointer address(const_reference val) const	

����ռ䡣����malloc��pHint�������ӣ���Ҫ�Ǹ����ʹ�ã�����������ܡ�
pointer allocate(size_type cnt, CHxAllocator<void>::const_pointer pHint = 0)	

�ͷſռ䣬����free��
void deallocate(pointer p, size_type n)	

�ɷ�������������
size_type max_size() const throw()	

�ڵ�ַp��ָ��Ŀռ䣬ʹ��val������䡣��Ҫʹ�õ�palcement new���Ա㱣֤���õ����캯����
void construct(pointer p, const_reference val)	

����pָ����ڴ�������ݡ�һ��ͨ����ʾ��������������ִ�С�
void destroy(pointer p)	

���ֹ��캯������������
allocator() throw ()
allocator(const_reference) throw ()
template <typename _other> allocator(CHxAllocator <_other> const&) throw()
~CHxAllocator() throw()

���ʵ��������Щ��������ֻҪ�ճ���׼���е�ʵ�־Ϳ����ˡ��������Ҫ��c��malloc��free��ʵ�֣�Ҳ������ôд��

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

�����ϣ����Ǿͼ�ʵ����һ���Լ���allocator�����⣬������Щ����Ҫ�Ľӿں������㻹��Ҫʵ�ֱȽϲ�����==��!=��
������Щ�����ݱ�׼�ĵ�����ֱ�ӷ���true��false��

��дallocator����ҪĿ����Ϊ��������ܡ�������������������أ�ֱ��ʹ��Windows��HeapXXXX���ڴ�API��
��ʵ�����Լ���һ�¾ͻᷢ�֣����������������ԡ���Ϊͨ��new����ͨ��malloc�����ͨ��HeapAlloc����ֱ�ӵ���HeapAlloc�༸�仰��
���ʵ��һ�������ܵ�allocator����Ҫ����memory pool���뷨�����⣬��ݵ�stlԴ�������������SGI STL���������뷨ʵ�ֵ�һ��alloc


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
T* tmp = (T*)(::operator new((size_t)(size * sizeof(T)))); // size���������Ԫ������sizeof(T)ÿ��Ԫ�صĴ�С
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
new(p)T1(value); // ����placement new����ָ�����ڴ�λ��p����ʼ��T1���󣬳�ʼ��T1����ʱ����T1�ĸ��ƹ��캯��
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