/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++ (test suite)
|  |  |__   |  |  | | | |  version 3.6.1
|_____|_____|_____|_|___|  https://github.com/nlohmann/json

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2013-2019 Niels Lohmann <http://nlohmann.me>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "doctest_compatibility.h"

#include <nlohmann/json.hpp>
#include <list>

namespace
{

// a dumb map preserving insertion order
// written solely for the purpose of this test
// in real life, consider something like https://github.com/Tessil/ordered-map

template <typename Key, typename T, typename Alloc = std::allocator<std::pair<const Key, T>>>
class insertion_ordered_map
{
private:
    using data_t = std::list<std::pair<const Key, T>, Alloc>;
    data_t data;

public:
    using key_type = Key;
    using value_type = std::pair<const Key, T>;
    using size_type = typename data_t::size_type;
    using iterator = typename data_t::iterator;
    using const_iterator = typename data_t::const_iterator;

    size_type size() const { return data.size(); }
    size_type max_size() const { return data.max_size(); }
    bool empty() const { return data.empty(); }
    const_iterator cbegin() const { return data.cbegin(); }
    const_iterator cend() const { return data.cend(); }
    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    T& operator[](const key_type & k) {
        auto e = end();
        auto pos = std::find_if(begin(), e, [&] (std::pair<const Key, T> & p) { return p.first == k; });
        return (pos == e ? data.emplace(e, k, T{}) : pos)->second;
    }
    iterator erase(const_iterator pos) { return data.erase(pos); }
};
}

namespace nlohmann
{
    template <class StringType, template<typename> class AllocatorType, class BasicJsonType>
    struct object_traits <insertion_ordered_map, StringType, AllocatorType, BasicJsonType>
    {
        using object_t = insertion_ordered_map<StringType, BasicJsonType, AllocatorType<std::pair<const StringType, BasicJsonType>>>;
    };
}

TEST_CASE("use of unordered maps")
{
    SECTION("order is stable with insertion_ordered_map")
    {
        const auto text = "{\"name\":1,\"id\":2}";
        CHECK(text == nlohmann::basic_json<insertion_ordered_map>::parse(text).dump());
    }

    SECTION("keys are sorted with std::map")
    {
        const auto text = "{\"name\":1,\"id\":2}";
        CHECK("{\"id\":2,\"name\":1}" == nlohmann::json::parse(text).dump());
    }
}
