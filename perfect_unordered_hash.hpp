#ifndef PERFECT_UNORDERED_HASH_HPP
#define PERFECT_UNORDERED_HASH_HPP

#include<memory>
#include<functional>
#include<random>
#include<algorithm>
#include<vector>
#include<numeric>
#include<unordered_set>
#include <iterator>

template<
	class Key, 
	class Hash = std::hash<Key>,
	class Prng = std::mt19937_64,
	class Allocator = std::allocator<size_t>
>
class perfect_unordered_hash
{
public:
	using key_type = Key;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using hasher = Hash;
	using prng = Prng;
	using allocator_type = Allocator;

private:
	template<class Iterator>
	void prealloc_vec(std::false_type,std::vector<size_t, allocator_type>& alloc, Iterator b, Iterator e)
	{}
	template<class Iterator>
	void prealloc_vec(std::true_type, std::vector<size_t, allocator_type>& alloc, Iterator b, Iterator e)
	{
		alloc.reserve(std::distance(b, e));
	}

	
public:
	//http ://cmph.sourceforge.net/papers/esa09.pdf
	std::vector<size_t, allocator_type> Larray;
	size_t M;
	Hash hash;

	template<typename Iterator>
	perfect_unordered_hash(Iterator b, Iterator e,size_t tM=0,
		const Hash& thash=Hash(),
		const Allocator& alloc=Allocator()
		):hash(thash)
	{
		std::vector<size_t, allocator_type> l1hashes(alloc);
		using is_ra_tag = std::integral_constant<bool, std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>::value>;
		prealloc_vec(is_ra_tag{},l1hashes, b, e);
		std::transform(b, e, std::back_inserter(l1hashes), hash);

		size_t lR = 7;
		size_t R = 1ULL << lR; //must be a power of 2
		//intermediate size
		size_t N = l1hashes.size();
		M = tM < N ? N : tM;
		
		//TODO allocator2
		std::vector<std::vector<size_t, allocator_type>> Bsets(R, std::vector<size_t, allocator_type>(alloc));
		size_t capest = (N * 4) / (R * 3);
		for (auto& bs : Bsets)
		{
			bs.reserve(capest);
		}
		for (size_t x : l1hashes)
		{
			Bsets[x & (R - 1)].push_back(x);
		}
		std::vector<size_t, allocator_type> rg(R);
		std::iota(rg.begin(), rg.end(), 0);
		std::sort(rg.begin(),rg.end(), [&Bsets](size_t a,size_t b)
		{
			return Bsets[a].size() > Bsets[b].size();
		});
		//TODO: vector Bool
		/*std::vector<size_t, allocator_type> T(m, 0, alloc);*/
		/*std::vector<size_t, allocator_type> Tcur(m, 0, alloc);*/

		std::vector<bool> T(M,false);
		std::unordered_set<size_t> Tcur;
		Larray = std::vector<size_t, allocator_type>(rg.size());

		for (auto i : rg)
		{
			std::vector<size_t, allocator_type>& B = Bsets[i];
			Prng prng;
			for (size_t l = 0; true; l++)
			{
				size_t li = prng();
				Tcur.clear();
				for (auto x : B)
				{
					size_t cx = (hash(x) ^ li) % M;
					if (!Tcur.insert(cx).second || T[cx]) break;
				}
				if (Tcur.size() == B.size())
				{
					for (auto cx : Tcur)
					{
						T[cx] = true;
					}
					Larray[i] = li;
					break;
				}
			}
		}
	}

	size_t operator()(const Key& k) const
	{
		size_t x = hash(k);
		size_t li = Larray[x & (Larray.size() - 1)]
		size_t cx = (hash(x) ^ li) % M;
		return cx;
	}
};

#endif