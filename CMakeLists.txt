cmake_minimum_required(VERSION 3.16)

project(ComplexWithTemplates2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ComplexWithTemplates2 src/ComplexWithTemplatesMain.cpp
    src/Complex.h src/Complex.tpp
    src/Memory/NewHandlerGuard.h src/Memory/NewHandlerGuard.tpp
    src/Memory/ObjectStorage.h src/Memory/ObjectStorage.tpp
    src/Memory/AllocationBlock.h)

include(GNUInstallDirs)
install(TARGETS ComplexWithTemplates2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
