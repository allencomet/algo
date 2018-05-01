/*-
 * Copyright (c) 2013 Cosku Acay, http://www.coskuacay.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include <climits>
#include <cstddef>

// 只能用于简单数据的内存分配，类似于boost::pool
namespace ccalloc {

template <typename T, size_t BlockSize = 4096>
class MemoryPool {
public:
	/* Member types */
	typedef T               value_type;
	typedef T*              pointer;
	typedef T&              reference;
	typedef const T*        const_pointer;
	typedef const T&        const_reference;
	typedef size_t          size_type;
	typedef ptrdiff_t       difference_type;
	typedef std::false_type propagate_on_container_copy_assignment;
	typedef std::true_type  propagate_on_container_move_assignment;
	typedef std::true_type  propagate_on_container_swap;

	// 定义 rebind<U>::other 接口
	template <typename U> struct rebind {
		typedef MemoryPool<U> other;
	};

	/* Member functions */
	// 默认构造, 初始化所有的槽指针
	// C++11 使用了 noexcept 来显式的声明此函数不会抛出异常
	MemoryPool() noexcept;
	MemoryPool(const MemoryPool& memoryPool) noexcept;
	MemoryPool(MemoryPool&& memoryPool) noexcept;
	template <class U> MemoryPool(const MemoryPool<U>& memoryPool) noexcept;

	// 销毁一个现有的内存池
	~MemoryPool() noexcept;

	MemoryPool& operator=(const MemoryPool& memoryPool) = delete;
	MemoryPool& operator=(MemoryPool&& memoryPool) noexcept;

	pointer address(reference x) const noexcept;
	const_pointer address(const_reference x) const noexcept;

	// Can only allocate one object at a time. n and hint are ignored
	pointer allocate(size_type n = 1, const_pointer hint = 0);	// 同一时间只能分配一个对象, n 和 hint 会被忽略
	void deallocate(pointer p, size_type n = 1);	// 销毁指针 p 指向的内存区块

	size_type max_size() const noexcept;

	// 调用构造函数
	template <class U, class... Args> void construct(U* p, Args&&... args);
	// 销毁内存池中的对象, 即调用对象的析构函数
	template <class U> void destroy(U* p);

	template <class... Args> pointer newElement(Args&&... args);
	void deleteElement(pointer p);

private:
	// 用于存储内存池中的对象槽, 
	// 要么被实例化为一个存放对象的槽, 
	// 要么被实例化为一个指向存放对象槽的槽指针
	union Slot_ {
		value_type element;
		Slot_* next;
	};

	typedef char* data_pointer_;	// 数据指针
	typedef Slot_ slot_type_;		// 对象槽
	typedef Slot_* slot_pointer_;	// 对象槽指针

	slot_pointer_ currentBlock_;	// 指向当前内存区块的指针 
	slot_pointer_ currentSlot_;		// 指向当前内存区块中的对象槽
	slot_pointer_ lastSlot_;		// 指向当前内存区块中的最后一个对象槽
	slot_pointer_ freeSlots_;		// 指向当前内存区块中所有空闲的对象槽

	size_type padPointer(data_pointer_ p, size_type align) const noexcept;
	void allocateBlock();

	// 检查定义的内存池大小是否过小
	// C++0x中引入了static_assert这个关键字，用来做编译期间的断言，因此叫做静态断言
	static_assert(BlockSize >= 2 * sizeof(slot_type_), "BlockSize too small.");
};

// #include "ccalloc.tcc"

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::size_type
MemoryPool<T, BlockSize>::padPointer(data_pointer_ p, size_type align)
const noexcept {
	uintptr_t result = reinterpret_cast<uintptr_t>(p);
	return ((align - result) % align);
}


// 默认构造函数
template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool()
noexcept {
	currentBlock_ = nullptr;
	currentSlot_ = nullptr;
	lastSlot_ = nullptr;
	freeSlots_ = nullptr;
}


// 拷贝构造函数
template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool(const MemoryPool& memoryPool)
noexcept : MemoryPool() {
}


// 移动构造函数
template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool(MemoryPool&& memoryPool)
noexcept {
	currentBlock_ = memoryPool.currentBlock_; // 移交所有权
	memoryPool.currentBlock_ = nullptr;
	currentSlot_ = memoryPool.currentSlot_;
	lastSlot_ = memoryPool.lastSlot_;
	freeSlots_ = memoryPool.freeSlots;
}

// 不同类型的构造函数
template <typename T, size_t BlockSize>
template<class U>
MemoryPool<T, BlockSize>::MemoryPool(const MemoryPool<U>& memoryPool)
noexcept : MemoryPool() {
}


// 重载移动赋值操作符
template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>&
MemoryPool<T, BlockSize>::operator=(MemoryPool&& memoryPool)
noexcept {
	if (this != &memoryPool) {
		std::swap(currentBlock_, memoryPool.currentBlock_);	// 交换内存块
		currentSlot_ = memoryPool.currentSlot_;
		lastSlot_ = memoryPool.lastSlot_;
		freeSlots_ = memoryPool.freeSlots;
	}
	return *this;
}

// 析构函数
template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::~MemoryPool()
noexcept {
	slot_pointer_ curr = currentBlock_;
	while (curr != nullptr) {
		slot_pointer_ prev = curr->next;
		operator delete(reinterpret_cast<void*>(curr));
		curr = prev;
	}
}

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::address(reference x)
const noexcept {
	return &x;
}

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::const_pointer
MemoryPool<T, BlockSize>::address(const_reference x)
const noexcept {
	return &x;
}

// 分配内存块
template <typename T, size_t BlockSize>
void MemoryPool<T, BlockSize>::allocateBlock() {
	// Allocate space for the new block and store a pointer to the previous one
	// 分配一个新的内存区块，并指向前一个内存区块
	data_pointer_ newBlock = reinterpret_cast<data_pointer_>
		(operator new(BlockSize));
	reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
	currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);

	// Pad block body to staisfy the alignment requirements for elements
	// 填补整个区块来满足元素内存区域的对齐要求
	data_pointer_ body = newBlock + sizeof(slot_pointer_);
	size_type bodyPadding = padPointer(body, alignof(slot_type_));
	currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
	lastSlot_ = reinterpret_cast<slot_pointer_>
		(newBlock + BlockSize - sizeof(slot_type_) + 1);
}

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::allocate(size_type n, const_pointer hint) {
	// 如果有空闲的对象槽，那么直接将空闲区域交付出去
	if (freeSlots_ != nullptr) {
		pointer result = reinterpret_cast<pointer>(freeSlots_);
		freeSlots_ = freeSlots_->next;
		return result;
	} else {
		// 如果对象槽不够用了，则分配一个新的内存区块
		if (currentSlot_ >= lastSlot_)
			allocateBlock();
		return reinterpret_cast<pointer>(currentSlot_++);
	}
}

template <typename T, size_t BlockSize>
inline void
MemoryPool<T, BlockSize>::deallocate(pointer p, size_type n) {
	if (p != nullptr) {
		reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
		freeSlots_ = reinterpret_cast<slot_pointer_>(p);
	}
}

// the maximum capacity current system can allocate
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::size_type
MemoryPool<T, BlockSize>::max_size()
const noexcept {
	size_type maxBlocks = -1 / BlockSize;
	return (BlockSize - sizeof(data_pointer_)) / sizeof(slot_type_) * maxBlocks;
}

// 调用构造函数构造数据
template <typename T, size_t BlockSize>
template <class U, class... Args>
inline void
MemoryPool<T, BlockSize>::construct(U* p, Args&&... args) {
	new (p) U(std::forward<Args>(args)...);// c++11 完美转发（在指定的位置p创建对象）
}

// 调用析构函数销毁结构数据
template <typename T, size_t BlockSize>
template <class U>
inline void
MemoryPool<T, BlockSize>::destroy(U* p) {
	p->~U();
}

// 分配内存空间 + 构造数据
template <typename T, size_t BlockSize>
template <class... Args>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::newElement(Args&&... args) {
	pointer result = allocate();
	construct<value_type>(result, std::forward<Args>(args)...);
	return result;
}

// 销毁数据 + 回收内存空间
template <typename T, size_t BlockSize>
inline void
MemoryPool<T, BlockSize>::deleteElement(pointer p) {
	if (p != nullptr) {
		p->~value_type();
		deallocate(p);
	}
}

}//namespace ccalloc