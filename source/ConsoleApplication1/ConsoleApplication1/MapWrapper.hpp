#ifndef __MAP_WRAPPER_HPP__
#define __MAP_WRAPPER_HPP__

/**
 * 提供给 unordered_map 的 for each 语法的遍历接口
 */
template <typename T>
class MapWrapper {
public:
	typedef std::unordered_map<int, T*> TMap;

	class Iterator {
	public:
		auto operator*() -> T& {
			return *_iter->second;
		}

		auto operator++() -> Iterator& {
			_iter++;
			return *this;
		}

		auto operator++(int) -> Iterator {
			auto cpy = *this;
			_iter++;
			return cpy;
		}

		auto operator==(const Iterator &rhs) const -> bool {
			return _iter == rhs._iter;
		}

		auto operator!=(const Iterator &rhs) const -> bool {
			return _iter != rhs._iter;
		}

	private:
		friend class MapWrapper<T>;

		typedef typename TMap::iterator TIterator;

		Iterator(const TIterator &iter) : _iter(iter) {}

		TIterator _iter;
	};

	MapWrapper(TMap &map) : _map(map) {}

	auto begin() -> Iterator {
		return _map.begin();
	}

	auto end() -> Iterator {
		return _map.end();
	}

private:
	TMap &_map;
};

#endif  // __MAP_WRAPPER_HPP__