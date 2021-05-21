#include "hello.hpp"

std::string hello(const char* name) {
    return std::string{"hello, "} + name;
}
