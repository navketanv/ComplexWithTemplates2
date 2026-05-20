#include <cassert>

template<typename T, typename Allocator>
template<typename... Args>
ObjectStorage<T, Allocator>::ObjectStorage(std::in_place_t, Args&&... args) {
    m_block = makeBlock(std::forward<Args>(args)...);
}

template<typename T, typename Allocator>
ObjectStorage<T, Allocator>::ObjectStorage(ObjectStorage<T, Allocator>&& rhs) noexcept(std::is_nothrow_move_constructible_v<Allocator>)
    : m_block(std::move(rhs.m_block)) {}

template<typename T, typename Allocator>
ObjectStorage<T, Allocator>& ObjectStorage<T, Allocator>::operator=(ObjectStorage<T, Allocator>&& rhs) noexcept(std::is_nothrow_move_assignable_v<Allocator>) {
    if (this != &rhs) {
        m_block = std::move(rhs.m_block);
    }
    return *this;
}

template<typename T, typename Allocator>
template<typename... Args>
void ObjectStorage<T, Allocator>::create(Args&&... args) {
    assert(m_block.data() == nullptr);
    m_block = makeBlock(std::forward<Args>(args)...);
}

template<typename T, typename Allocator>
template<typename... Args>
void ObjectStorage<T, Allocator>::emplace(Args&&... args) {
    m_block = makeBlock(std::forward<Args>(args)...);
}

template<typename T, typename Allocator>
template<typename... Args>
AllocationBlock<T, Allocator> ObjectStorage<T, Allocator>::makeBlock(Args&&... args) {
    Allocator& allocator = m_block.allocator();
    T* ptr = std::allocator_traits<Allocator>::allocate(allocator, 1);
    try {
        std::allocator_traits<Allocator>::construct(allocator, ptr, std::forward<Args>(args)...);
    } catch (...) {
        std::allocator_traits<Allocator>::deallocate(allocator, ptr, 1);
        throw;
    }
    return AllocationBlock<T, Allocator>(ptr, allocator, 1);
}

template<typename T, typename Allocator>
void ObjectStorage<T, Allocator>::reset() noexcept {
    m_block.reset();
}

template<typename T, typename Allocator>
T* ObjectStorage<T, Allocator>::get() noexcept {
    return m_block.data();
}

template<typename T, typename Allocator>
const T* ObjectStorage<T, Allocator>::get() const noexcept {
    return m_block.data();
}

template<typename T, typename Allocator>
T& ObjectStorage<T, Allocator>::operator*() noexcept {
    assert(m_block.data() != nullptr);
    return *(m_block.data());
}

template<typename T, typename Allocator>
const T& ObjectStorage<T, Allocator>::operator*() const noexcept {
    assert(m_block.data() != nullptr);
    return *(m_block.data());
}

template<typename T, typename Allocator>
T* ObjectStorage<T, Allocator>::operator->() noexcept {
    assert(m_block.data() != nullptr);
    return m_block.data();
}

template<typename T, typename Allocator>
const T* ObjectStorage<T, Allocator>::operator->() const noexcept {
    assert(m_block.data() != nullptr);
    return m_block.data();
}

template<typename T, typename Allocator>
T& ObjectStorage<T, Allocator>::value() noexcept {
    assert(m_block.data() != nullptr);
    return *(m_block.data());
}

template<typename T, typename Allocator>
const T& ObjectStorage<T, Allocator>::value() const noexcept {
    assert(m_block.data() != nullptr);
    return *(m_block.data());
}

template<typename T, typename Allocator>
ObjectStorage<T, Allocator>::operator bool() const noexcept {
    return (m_block.data() != nullptr);
}
