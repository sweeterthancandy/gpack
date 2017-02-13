#pragma once

#include <bitset>

#include <boost/type_index.hpp>

#define PRINT(X) do{ std::cout << #X " => " << (X) << "\n";}while(0)
#define PRINT_BYTE(X) do{ std::cout << #X " => " << std::bitset<8>(X).to_string() << "\n";}while(0)
#define PRINT_BINARY(X) do{ std::cout << #X " => " << std::bitset<8 * sizeof(X)>(X).to_string() << "\n";}while(0)
#define PRINT_TYPE(X) do{ std::cout << #X " => " << boost::typeindex::type_id_runtime(X).pretty_name() << "\n";}while(0)
