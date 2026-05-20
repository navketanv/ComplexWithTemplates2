# ComplexWithTemplates2
Basic Complex Number Lib With AllocationBlock and BlockStorage
#pragma once
#include <utility>
#include <memory>
#include <cstddef>
#include <type_traits>

template<typename T, typename Allocator>
class AllocationBlock {
public:
    AllocationBlock() = default;
    explicit AllocationBlock(T* ptr, Allocator alloc, size_t count = 1)
        : m_ptr(ptr)
        , m_allocator(std::move(alloc))
        , m_count(count) {}

    AllocationBlock(AllocationBlock&& rhs) noexcept(std::is_nothrow_move_constructible_v<Allocator>)
        : m_ptr(std::exchange(rhs.m_ptr, nullptr))
        , m_allocator(std::move(rhs.m_allocator))
        , m_count(std::exchange(rhs.m_count, 0)) {}

    AllocationBlock& operator=(AllocationBlock&& rhs) noexcept(std::is_nothrow_move_assignable_v<Allocator>) {
        if (this != &rhs) {
            destroy();
            m_ptr = std::exchange(rhs.m_ptr, nullptr);
            m_allocator = std::move(rhs.m_allocator);
            m_count = std::exchange(rhs.m_count, 0);
        }
        return *this;
    }

    AllocationBlock(const AllocationBlock& rhs) = delete;
    AllocationBlock& operator=(const AllocationBlock& rhs) = delete;
    ~AllocationBlock() noexcept {
        destroy();
    }

    void reset() noexcept {
        destroy();
    }

    T* release() noexcept {
        m_count = 0;
        return std::exchange(m_ptr, nullptr);
    }

    [[nodiscard]] constexpr T* data() noexcept {
        return m_ptr;
    }

    [[nodiscard]] constexpr const T* data() const noexcept {
        return m_ptr;
    }

    [[nodiscard]] constexpr Allocator& allocator() noexcept {
        return m_allocator;
    }

    [[nodiscard]] constexpr const Allocator& allocator() const noexcept {
        return m_allocator;
    }

    [[nodiscard]] constexpr size_t count() const noexcept {
        return m_count;
    }

private:
    void destroy() noexcept {
        if (m_ptr) {
            for (size_t index = 0; index < m_count; ++index) {
                std::allocator_traits<Allocator>::destroy(m_allocator, m_ptr + index);
            }
            std::allocator_traits<Allocator>::deallocate(m_allocator, m_ptr, m_count);
            m_ptr = nullptr;
            m_count = 0;
        }
    }

private:
    T* m_ptr{nullptr};
    [[no_unique_address]] Allocator m_allocator{};
    size_t m_count{0};
};

#pragma once
#include "AllocationBlock.h"
#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class ObjectStorage
{
public:
    ObjectStorage() = default;
    ObjectStorage(const ObjectStorage&) = delete;
    ObjectStorage& operator=(const ObjectStorage&) = delete;

    template<typename... Args>
    explicit ObjectStorage(std::in_place_t, Args&&... args);
    ObjectStorage(ObjectStorage&& rhs) noexcept(std::is_nothrow_move_constructible_v<Allocator>);
    ObjectStorage& operator=(ObjectStorage&& rhs) noexcept(std::is_nothrow_move_assignable_v<Allocator>);
    ~ObjectStorage() noexcept = default;

public:
    template<typename... Args>
    void create(Args&&... args);

    template<typename... Args>
    void emplace(Args&&... args);

    void reset() noexcept;

    [[nodiscard]] T* get() noexcept;
    [[nodiscard]] const T* get() const noexcept;

    T& operator*() noexcept;
    const T& operator*() const noexcept;

    T* operator->() noexcept;
    const T* operator->() const noexcept;

    T& value() noexcept;
    const T& value() const noexcept;

    [[nodiscard]] explicit operator bool() const noexcept;

private:
    template<typename... Args>
    AllocationBlock<T, Allocator> makeBlock(Args&&... args);

private:
    AllocationBlock<T, Allocator> m_block{};
};

#include "ObjectStorage.tpp"

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

