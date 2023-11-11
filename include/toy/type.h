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
#include <cstdint>
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
using BinaryHeat=std::priority_queue<T>;
using String=std::string;
#endif /* INCLUDE_TOY_TYPE */
