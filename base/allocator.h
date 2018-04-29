#include "base.h"
#include "data_types.h"

#include <jemalloc/jemalloc.h>	// �ڱ���ʱ������-DJEMALLOC_NO_DEMANGLE -static

namespace mem {

namespace stdmem {
template <class T>
class malloc_allocator {
public:
	typedef T                 value_type;
	typedef value_type*       pointer;
	typedef const value_type* const_pointer;
	typedef value_type&       reference;
	typedef const value_type& const_reference;
	typedef std::size_t       size_type;
	typedef std::ptrdiff_t    difference_type;		// ptrdiff_t���ͱ���ͨ��������������ָ����������Ľ��

	template <class U>
	struct rebind { typedef malloc_allocator<U> other; };

	// default constructor
	malloc_allocator() {}
	// copy constructor
	malloc_allocator(const malloc_allocator&) {}

	// copy constructor for different type
	template <class U>
	malloc_allocator(const malloc_allocator<U>&) {}

	// destructor
	~malloc_allocator() {}

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const {
		return x;
	}

	pointer allocate(size_type n, const_pointer = 0) {
		void* p = std::malloc(n * sizeof(T));
		if (!p) throw std::bad_alloc();

		return static_cast<pointer>(p);
	}

	void deallocate(pointer p, size_type) {
		std::free(p);
	}

	// the maximum capacity current system can allocate
	size_type max_size() const {
		return static_cast<size_type>(-1) / sizeof(T);
	}

	void construct(pointer p, const value_type& x) {
		new(p) value_type(x);	// ��ָ����λ��p��������
	}

	void destroy(pointer p) { p->~value_type(); }

private:
	void operator=(const malloc_allocator&) = delete;
};

template<>
class malloc_allocator<void> {
	typedef void        value_type;
	typedef void*       pointer;
	typedef const void* const_pointer;

	template <class U>
	struct rebind { typedef malloc_allocator<U> other; };
};

// ��������malloc_allocator���ǿɻ�����
// ��������ıȽϲ����Ա������е�malloc_allocator�����ǵȼ۵�

template <class T>
inline bool operator==(const malloc_allocator<T>&,
	const malloc_allocator<T>&) {
	return true;
}

template <class T>
inline bool operator!=(const malloc_allocator<T>&,
	const malloc_allocator<T>&) {
	return false;
}

}//namespace stdmem

namespace jemem {

template <class T>
class malloc_allocator {
public:
	typedef T                 value_type;
	typedef value_type*       pointer;
	typedef const value_type* const_pointer;
	typedef value_type&       reference;
	typedef const value_type& const_reference;
	typedef std::size_t       size_type;
	typedef std::ptrdiff_t    difference_type;		// ptrdiff_t���ͱ���ͨ��������������ָ����������Ľ��

	template <class U>
	struct rebind { typedef malloc_allocator<U> other; };

	// default constructor
	malloc_allocator() {}
	// copy constructor
	malloc_allocator(const malloc_allocator&) {}

	// copy constructor for different type
	template <class U>
	malloc_allocator(const malloc_allocator<U>&) {}

	// destructor
	~malloc_allocator() {}

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const {
		return x;
	}

	pointer allocate(size_type n, const_pointer = 0) {
		void* p = je_malloc(n * sizeof(T));
		if (!p) throw std::bad_alloc();

		return static_cast<pointer>(p);
	}

	void deallocate(pointer p, size_type) {
		je_free(p);
	}

	// the maximum capacity current system can allocate
	size_type max_size() const {
		return static_cast<size_type>(-1) / sizeof(T);
	}

	void construct(pointer p, const value_type& x) {
		new(p) value_type(x);	// ��ָ����λ��p��������
	}

	void destroy(pointer p) { p->~value_type(); }

private:
	void operator=(const malloc_allocator&) = delete;
};

template<>
class malloc_allocator<void> {
	typedef void        value_type;
	typedef void*       pointer;
	typedef const void* const_pointer;

	template <class U>
	struct rebind { typedef malloc_allocator<U> other; };
};

// ��������malloc_allocator���ǿɻ�����
// ��������ıȽϲ����Ա������е�malloc_allocator�����ǵȼ۵�

template <class T>
inline bool operator==(const malloc_allocator<T>&,
	const malloc_allocator<T>&) {
	return true;
}

template <class T>
inline bool operator!=(const malloc_allocator<T>&,
	const malloc_allocator<T>&) {
	return false;
}

}//namespace jemem

}//namespace mem