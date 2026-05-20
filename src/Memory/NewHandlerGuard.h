#pragma once
#include <new>

class NewHandlerGuard
{
public:
    explicit NewHandlerGuard(std::new_handler pHandler) noexcept;
    ~NewHandlerGuard() noexcept;

    NewHandlerGuard(const NewHandlerGuard&) = delete;
    NewHandlerGuard& operator=(const NewHandlerGuard&) = delete;

private:
    std::new_handler m_pPrevHandler{nullptr};
};

#include "NewHandlerGuard.tpp"