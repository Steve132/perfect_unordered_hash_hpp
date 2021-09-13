#include<algorithm>
#include<random>
#include<vector>
#include<chrono>
#include<iostream>

template<class Func>
double timeit(Func& f)
{
	auto start = std::chrono::high_resolution_clock::now();
	f();
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<double>(end - start).count();
}

template<class Iterator, class KeyFunc>
static void radix_internal(Iterator b, Iterator e, unsigned int logkp1, KeyFunc kf)
{
	size_t n = std::distance(b, e);
	if (n <= 1 || logkp1 == 0) return;
	unsigned int kmsk = 1UL << (logkp1 - 1);
	Iterator curiter = b;
	Iterator curb = b, cure = e - 1;
	for (size_t i = 0; i < n; i++)
	{
		if (kf(*curiter) & kmsk)
		{
			std::iter_swap(curiter, cure--);
			curiter = cure;
		}
		else
		{
			std::iter_swap(curiter, curb++);
			curiter = curb;
		}
	}
	radix_internal(b, curb, logkp1 - 1, kf);
	radix_internal(curb, e, logkp1 - 1, kf);
}

static void vsort(const std::vector<size_t>& dset, size_t R)
{
	std::vector<std::vector<size_t>> Bsets(R);
	size_t Ncap = (dset.size() / R)+1;
	for (auto& v : Bsets)
	{
		v.reserve(Ncap);
	}
	for (auto x : dset)
	{
		Bsets[x].push_back(x);
	}
}

int main(int argc, char** argv)
{
	size_t N = 100000000;
	size_t lR = 10;
	size_t R = 1ULL << lR;
	std::cout << "Generating dataset" << std::endl;
	std::vector<size_t> dataset(N);
	std::mt19937_64 mt;

	for (auto& d : dataset)
	{
		d = mt() & (R-1);
	}
	std::vector<size_t> dataset2=dataset;
	std::vector<size_t> dataset3 = dataset;

	std::cout << "Radix Sort: " << timeit(
		[&]()
		{
			radix_internal(dataset.begin(), dataset.end(), lR, [](size_t a) {return a; });
		}
	) << std::endl;

	std::cout << "StdSort: " << timeit(
		[&]()
		{
			std::sort(dataset2.begin(), dataset2.end());
		}
	) << std::endl;

	std::cout << "VPartition: " << timeit(
		[&]()
		{
			vsort(dataset3,R);
		}
	) << std::endl;

	std::cout << "Is equal: " << (int)std::equal(dataset.begin(), dataset.end(), dataset2.begin()) << std::endl;
	return 0;
}