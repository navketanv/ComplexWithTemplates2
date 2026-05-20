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