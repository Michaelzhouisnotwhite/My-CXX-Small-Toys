#ifndef INCLUDE_TOY_TYPE
#define INCLUDE_TOY_TYPE
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#define TYPE1 template<typename T>
#define TYPE2 template<typename T, typename A>
TYPE1
using Vec = std::vector<T>;
TYPE1
using VecDeque = std::deque<T>;
TYPE1
using LinkedList = std::list<T>;
TYPE2
using BTreeMap = std::map<T, A>;
TYPE2
using HashMap = std::unordered_map<T, A>;
TYPE1
using HashSet = std::unordered_set<T>;
TYPE1
using BTreeSet = std::set<T>;
TYPE1
using BinaryHeat = std::priority_queue<T>;
using String = std::string;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i64 = std::int64_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using i8 = std::int8_t;

using f32 = float;
using f64 = double;
#endif /* INCLUDE_TOY_TYPE */
