#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <stack>
#include <mutex>
#include <bitset>
#include <array>


template <typename T, size_t BlockSize = 4096>
class MemoryPool
{
public:
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

	MemoryPool() noexcept {
		inicialize();
	};
	MemoryPool(const MemoryPool& memoryPool) noexcept {
		inicialize();
	};
	MemoryPool(MemoryPool&& memoryPool) noexcept {
		currentBlock_ = memoryPool.currentBlock_;
		memoryPool.currentBlock_ = nullptr;
		currentSlot_ = memoryPool.currentSlot_;
		lastSlot_ = memoryPool.lastSlot_;
		freeSlots_ = memoryPool.freeSlots_;
	};
	//template <class U> MemoryPool(const MemoryPool<U>& memoryPool) noexcept {};

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
			freeSlots_ = memoryPool.freeSlots_;
		}
		return *this;
	};

	pointer address(reference x) const noexcept { return &x; };
	const_pointer address(const_reference x) const noexcept { return &x; };

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
	void inicialize() {
		currentBlock_ = nullptr;
		currentSlot_ = nullptr;
		lastSlot_ = nullptr;
		freeSlots_ = nullptr;
	}
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
		data_pointer_ newBlock = reinterpret_cast<data_pointer_>
			(operator new(BlockSize));
		reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
		currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
		data_pointer_ body = newBlock + sizeof(slot_pointer_);
		size_type bodyPadding = padPointer(body, alignof(slot_type_));
		currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
		lastSlot_ = reinterpret_cast<slot_pointer_>
			(newBlock + BlockSize - sizeof(slot_type_) + 1);
	};

	static_assert(BlockSize >= 2 * sizeof(slot_type_), "BlockSize too small.");
};

template <class T, class U>
constexpr bool operator== (const MemoryPool<T>& a1, const MemoryPool<U>& a2) noexcept
{
	return true;
}

template <class T, class U>
constexpr bool operator!= (const MemoryPool<T>&, const MemoryPool<U>&) noexcept
{
	return false;
}

template <typename T, typename Alloc = std::allocator<T>>
class MyList
{
	struct Node
	{
		Node* next;
		T val;
		Node(T _val) : val(_val), next(nullptr) {}
	};
public:
	MyList() : first(nullptr), last(nullptr) {
	}
	void push_back(const T& _val)
	{
		Node* p = nodeAlloc.allocate(1);
		p->val = _val;
		//Node* p = new Node(_val);
		if (is_empty()) {
			first = p;
			last = p;
			return;
		}
		last->next = p;
		last = p;
	}

	void print() {
		Node* temp = first;

		if (temp == NULL)
		{
			std::cout << "";
		}

		else if (temp->next == NULL)
		{
			std::cout << temp->val;
		}
		else
		{
			while (temp != NULL)
			{
				std::cout << temp->val;
				temp = temp->next;
				std::cout << std::endl;
			}
		}
	}
private:
	Node* first;
	Node* last;
	typename Alloc::template rebind<Node>::other nodeAlloc;
	bool is_empty() {
		return first == nullptr;
	}
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

		auto allocated_m = std::map<int, int, std::less<int>, MemoryPool<std::pair<const int, int>>>{};
		for (size_t i = 0; i < 10; i++)
			allocated_m[i] = factorial(i);
		for (const auto& p : allocated_m)
		{
			std::cout << p.first << " " << p.second << std::endl;
		}

		MyList<int> simple_l;
		for (size_t i = 0; i < 10; i++)
			simple_l.push_back(i);

		MyList<int, MemoryPool<int>> allocated_l;
		for (size_t i = 0; i < 10; i++)
			allocated_l.push_back(i);
		allocated_l.print();


		auto allocated_m2 = allocated_m;
		allocated_m2[2] = 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
