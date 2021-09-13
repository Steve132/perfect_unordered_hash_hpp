cmake_minimum_required(VERSION 3.10)

project(perfect_unordered_hash)

add_executable(
	test1
	perfect_unordered_hash.hpp
	test1.cpp
)

add_executable(radix_test
	radix_test.cpp
)