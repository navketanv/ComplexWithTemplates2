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
