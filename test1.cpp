#include "perfect_unordered_hash.hpp"

int main(int argc, char** argv)
{
	std::vector<size_t> tdata = { 430,23,1,55,5,1 };
	perfect_unordered_hash<size_t> phsh(tdata.begin(), tdata.end());
	return 0;
}

