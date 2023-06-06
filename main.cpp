#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <stack>
#include <mutex>
#include <bitset>
#include <array>

template <class T>
class Pool_c
{ // Basic type define
public:
	template <class U> Pool_c(const Pool_c <U>&) noexcept {}

	typedef unsigned int uint;


	uint m_numOfBlocks; // Num of blocks
	uint m_sizeOfEachBlock; // Size of each block
	uint m_numFreeBlocks; // Num of remaining blocks
	uint m_numInitialized; // Num of initialized blocks
	T* m_memStart; // Beginning of memory pool
	T* m_next; // Num of next free block
	typedef T value_type;
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	Pool_c()
	{
		m_numOfBlocks = 0;
		m_sizeOfEachBlock = 0;
		m_numFreeBlocks = 0;
		m_numInitialized = 0;
		m_memStart = NULL;
		m_next = 0;
		CreatePool();
	}
	~Pool_c() { DestroyPool(); }
	void CreatePool(size_t sizeOfEachBlock = sizeof(T),
		uint numOfBlocks = 10)
	{
		m_numOfBlocks = numOfBlocks;
		m_sizeOfEachBlock = sizeOfEachBlock;
		m_memStart = new T[m_sizeOfEachBlock *
			m_numOfBlocks];
		m_numFreeBlocks = numOfBlocks;
		m_next = m_memStart;
	}
	void DestroyPool()
	{
		delete[] m_memStart;
		m_memStart = NULL;
	}
	T* AddrFromIndex(uint i) const
	{
		return m_memStart + (i * m_sizeOfEachBlock);
	}
	uint IndexFromAddr(const T* p) const
	{
		return (((uint)(p - m_memStart)) / m_sizeOfEachBlock);
	}
	void* allocate(int n)
	{
		if (m_numInitialized < m_numOfBlocks)
		{
			uint* p = (uint*)AddrFromIndex(m_numInitialized);
			*p = m_numInitialized + 1;
			m_numInitialized++;
		}
		void* ret = NULL;
		if (m_numFreeBlocks > 0)
		{
			ret = (void*)m_next;
			--m_numFreeBlocks;
			if (m_numFreeBlocks != 0)
			{
				m_next = AddrFromIndex(*((uint*)m_next));
			}
			else
			{
				m_next = NULL;
			}
		}
		return ret;
	}
	void deallocate(void* p)
	{
		if (m_next != NULL)
		{
			(*(uint*)p) = IndexFromAddr(m_next);
			m_next = (T*)p;
		}
		else
		{
			*((uint*)p) = m_numOfBlocks;
			m_next = (T*)p;
		}
		++m_numFreeBlocks;
	}
	template< class U >
	struct rebind
	{
		typedef Pool_c<U> other;
	};
};

template <class T>
struct std_03_allocator {
	typedef T value_type;
	typedef T* pointer;

	T* pool = nullptr;
	unsigned int max_size = 10;
	unsigned int free_slots_count = 10;
	T* internal_array;
	bool* flags;

	std_03_allocator() noexcept {
		internal_array = new T[max_size];
		flags = new bool[max_size] {true};
		//pool = static_cast<T*>(::operator new(10 * sizeof(T)));
	}
	template <class U> std_03_allocator(const std_03_allocator <U>&) noexcept {}

	unsigned int get_free_index() {
		if (this->flags == nullptr) return -1;
		for (std::size_t i = 0; i < max_size; ++i) {
			if (flags[i]) return i;
		}
		return -1;
	}

	T* allocate(std::size_t n)
	{
		unsigned int index = get_free_index();
		if (this->free_slots_count <= 0) {
			std::cout << this->free_slots_count << std::endl;
		}
		if (this->pool != nullptr) {
			T* element_pointer = static_cast<T*>(this->pool + sizeof(T) * (10 - this->free_slots_count));

			this->free_slots_count--;
			return element_pointer;
		}
		else {
			this->pool = static_cast<T*>(::operator new(10 * sizeof(T)));
			this->free_slots_count = 10;
			return this->pool;
		}
		//return static_cast<T*>(::operator new(n * sizeof(T)));
	}
	void deallocate(T* p, std::size_t n)
	{
		//::operator delete[](p);
	}

	template <class Up, class... Args>
	void construct(Up* p, Args&&... args) {
		::new ((void*)p) Up(std::forward<Args>(args)...);
	}

	void destroy(pointer p) {
		//p->~T();
	}

	template< class U >
	struct rebind
	{
		typedef std_03_allocator<U> other;
	};
};

template <typename T, size_t BlockSize = 4096>
class MemoryPool
{
public:
	/* Member types */
	typedef T               value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef const T* const_pointer;
	typedef const T& const_reference;
	typedef size_t          size_type;
	typedef ptrdiff_t       difference_type;
	typedef std::false_type propagate_on_container_copy_assignment;
	typedef std::true_type  propagate_on_container_move_assignment;
	typedef std::true_type  propagate_on_container_swap;

	template <typename U> struct rebind {
		typedef MemoryPool<U> other;
	};

	/* Member functions */
	MemoryPool() noexcept {
		currentBlock_ = nullptr;
		currentSlot_ = nullptr;
		lastSlot_ = nullptr;
		freeSlots_ = nullptr;
	};
	MemoryPool(const MemoryPool& memoryPool) noexcept {};
	MemoryPool(MemoryPool&& memoryPool) noexcept {
		currentBlock_ = memoryPool.currentBlock_;
		memoryPool.currentBlock_ = nullptr;
		currentSlot_ = memoryPool.currentSlot_;
		lastSlot_ = memoryPool.lastSlot_;
		freeSlots_ = memoryPool.freeSlots;
	};
	template <class U> MemoryPool(const MemoryPool<U>& memoryPool) noexcept {};

	~MemoryPool() noexcept
	{
		slot_pointer_ curr = currentBlock_;
		while (curr != nullptr) {
			slot_pointer_ prev = curr->next;
			operator delete(reinterpret_cast<void*>(curr));
			curr = prev;
		}
	};

	MemoryPool& operator=(const MemoryPool& memoryPool) = delete;
	MemoryPool& operator=(MemoryPool&& memoryPool) noexcept {
		if (this != &memoryPool)
		{
			std::swap(currentBlock_, memoryPool.currentBlock_);
			currentSlot_ = memoryPool.currentSlot_;
			lastSlot_ = memoryPool.lastSlot_;
			freeSlots_ = memoryPool.freeSlots;
		}
		return *this;
	};

	pointer address(reference x) const noexcept { return &x; };
	const_pointer address(const_reference x) const noexcept { return &x; };

	// Can only allocate one object at a time. n and hint are ignored
	pointer allocate(size_type n = 1, const_pointer hint = 0) {
		if (freeSlots_ != nullptr) {
			pointer result = reinterpret_cast<pointer>(freeSlots_);
			freeSlots_ = freeSlots_->next;
			return result;
		}
		else {
			if (currentSlot_ >= lastSlot_)
				allocateBlock();
			return reinterpret_cast<pointer>(currentSlot_++);
		}
	};
	void deallocate(pointer p, size_type n = 1) {
		if (p != nullptr) {
			reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
			freeSlots_ = reinterpret_cast<slot_pointer_>(p);
		}
	};

	size_type max_size() const noexcept {
		//size_type maxBlocks = -1 / BlockSize;
		//return (BlockSize - sizeof(data_pointer_)) / sizeof(slot_type_) * maxBlocks;
		return 10;
	};

	template <class U, class... Args> void construct(U* p, Args&&... args) {
		new (p) U(std::forward<Args>(args)...);
	};
	template <class U> void destroy(U* p) { p->~U(); };

	template <class... Args> pointer newElement(Args&&... args) {
		pointer result = allocate();
		construct<value_type>(result, std::forward<Args>(args)...);
		return result;
	};
	void deleteElement(pointer p) {
		if (p != nullptr) {
			p->~value_type();
			deallocate(p);
		}
	};

private:
	union Slot_ {
		value_type element;
		Slot_* next;
	};

	typedef char* data_pointer_;
	typedef Slot_ slot_type_;
	typedef Slot_* slot_pointer_;

	slot_pointer_ currentBlock_;
	slot_pointer_ currentSlot_;
	slot_pointer_ lastSlot_;
	slot_pointer_ freeSlots_;

	size_type padPointer(data_pointer_ p, size_type align) const noexcept {
		uintptr_t result = reinterpret_cast<uintptr_t>(p);
		return ((align - result) % align);
	};
	void allocateBlock() {
		// Allocate space for the new block and store a pointer to the previous one
		data_pointer_ newBlock = reinterpret_cast<data_pointer_>
			(operator new(BlockSize));
		reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
		currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
		// Pad block body to staisfy the alignment requirements for elements
		data_pointer_ body = newBlock + sizeof(slot_pointer_);
		size_type bodyPadding = padPointer(body, alignof(slot_type_));
		currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
		lastSlot_ = reinterpret_cast<slot_pointer_>
			(newBlock + BlockSize - sizeof(slot_type_) + 1);
	};

	static_assert(BlockSize >= 2 * sizeof(slot_type_), "BlockSize too small.");
};


unsigned factorial(unsigned n)
{
	if (n < 0)
		return(-1);
	if (n == 0)
		return(1);
	else
	{
		return(n * factorial(n - 1));
	}
}
int main(int argc, char const* argv[])
{
	try
	{
		auto simple_m = std::map<int, int>{};

		for (size_t i = 0; i < 10; i++)
			simple_m[i] = factorial(i);

		auto allocated_m = std::map<int, int, MemoryPool<std::pair<int, int>>>{};
		for (size_t i = 0; i < 10; i++)
			allocated_m[i] = factorial(i);


		//auto v1 = std::vector<int, pool>{};

		//v1.push_back(1);
		//v1.push_back(2);
		//v1.push_back(3);
		//v1.push_back(4);
		//v1.push_back(5);
		//v1.push_back(6);
		//v1.push_back(7);
		//v1.push_back(8);
		//v1.push_back(9);
		//v1.push_back(0);
		//v1.push_back(0);
		//v1.push_back(0);
		//v1.push_back(0);
		//v1.push_back(0);
		//v1.push_back(0);

		//auto allocated_m = std::vector<int, pool_allocator<int>>{};
		//allocated_m.push_back(1);
		//allocated_m.push_back(2);
		//allocated_m.push_back(3);
		//allocated_m.push_back(4);
		//allocated_m.push_back(5);
		//allocated_m.push_back(6);
		//allocated_m.push_back(7);
		//allocated_m.push_back(8);
		////allocated_m.push_back(9);
		////allocated_m.push_back(0);

		//std::cout << allocated_m[0] << std::endl;
		//std::cout << allocated_m[1] << std::endl;
		//std::cout << allocated_m[2] << std::endl;



		//std::cout << allocated_m[7];

		//std::cout << allocated_l[0];
		//for (size_t i = 0; i < 3; i++)
		//	allocated_m[i] = factorial(i);

		/*for (const auto& n : allocated_m)
			std::cout << n.first << " = " << n.second << "; ";*/

	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
