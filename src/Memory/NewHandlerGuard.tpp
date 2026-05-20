#include <new>

NewHandlerGuard::NewHandlerGuard(std::new_handler pHandler) noexcept
    : m_pPrevHandler(std::set_new_handler(pHandler)) {}

NewHandlerGuard::~NewHandlerGuard() noexcept {
    std::set_new_handler(m_pPrevHandler);
}