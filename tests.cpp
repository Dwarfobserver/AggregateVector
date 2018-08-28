
#include "aggregate_vector.hpp"
#include <iostream>
#include <vector>
#include <string>


#define ASSERT_EQ(t, a, b) std::cout << std::string \
    {"["} + typeid(t).name() + "] At "#a" == "#b" : " + \
    std::to_string(a) + " == "  + std::to_string(b) \
    << std::endl; \
    if (a != b) throw std::logic_error{ #a" != "#b };

namespace user {
    struct physics_t {
        bool pos;
        double speed;
        float acc;
        int   id;
    };
}
AV_DEFINE_TYPE(user::physics_t, pos, speed, acc, id);

// Get iterator on component I/N of values from std::vector<T>.
template <size_t I, size_t N, class T>
auto make_component_view(std::vector<T> const& vec) {
    struct iterator {
        T const* ptr;

        bool operator==(iterator const& it) const { return ptr == it.ptr; }
        bool operator!=(iterator const& it) const { return ptr != it.ptr; }

        iterator& operator++() { ++ptr; return *this; }

        auto const& operator*() const {
            auto const tuple = av::detail::as_tuple<N>(*ptr);
            return std::get<I>(tuple);
        }
        auto const* operator->() const {
            auto const tuple = av::detail::as_tuple<N>(*ptr);
            return &std::get<I>(tuple);
        }
    };
    struct range {
        iterator begin_;
        iterator end_;

        int      size()  const { return end_.ptr - begin_.ptr; }
        iterator begin() const { return begin_; }
        iterator end()   const { return end_; }
    };
    return range{ iterator{ vec.data() }, iterator{ vec.data() + vec.size() } };
}

// derp
namespace std {
    template <class T>
    string to_string(T const&) { return "***"; }
}

// Compare size and values of a specific component from both vectors.
template <class Span, class Span2>
void check_vector_integrity(Span const& span, Span2 const& span_copy) {
    using T = std::remove_const_t<decltype(*span.data())>;
    ASSERT_EQ(T, span.size(), span_copy.size());
    auto it      = span.begin();
    auto it_copy = span_copy.begin();
    for (; it != span.end(); ++it, ++it_copy) {
        ASSERT_EQ(T, *it, *it_copy);
    }
    ASSERT_EQ(T, it_copy, span_copy.end());
}

// Compare size, capacity and values from both vectors.
template <class T, size_t...Is>
void check_vector_integrity(av::vector<T> const& vec, std::vector<T> const& vec_copy, std::index_sequence<Is...>) {
    ASSERT_EQ(T, vec.empty()   , vec_copy.empty());
    ASSERT_EQ(T, vec.size()    , vec_copy.size());
    ASSERT_EQ(T, vec.capacity(), vec_copy.capacity());
    (check_vector_integrity(
        vec.get_component<Is>(),
        make_component_view<Is, sizeof...(Is)>(vec_copy)
    ), ...);
    std::cout << std::endl;
}

template <class T>
struct vector_interface {
    av::vector<T>  v1;
    std::vector<T> v2;

    void check_integrity() const {
        constexpr auto size = av::vector<T>::components_count;
        using seq = std::make_index_sequence<size>;
        check_vector_integrity(v1, v2, seq{});
    }
};

template <class T>
void test_vector(T const& value) {
    #define TEST(n, ...) n.v1.__VA_ARGS__; n.v2.__VA_ARGS__; n.check_integrity()
    #define CHECK(n) n.check_integrity()

    auto i1 = vector_interface<T>{};
    CHECK(i1);
    TEST(i1, reserve(4));
    TEST(i1, resize(1));
    //TEST(i1, resize(7));
    TEST(i1, push_back(value));

    auto const j = i1;
    CHECK(j);

    auto i2 = std::move(i1);
    CHECK(i2);
    TEST(i2, emplace_back());
    TEST(i2, pop_back());
    TEST(i2, clear());

    #undef TEST
}

// TODO ctor(int, T?, alloc?)
// TODO ctor({ init_list })
// TODO test integrity
// TODO test performances (vs std::vector, with different access patterns)

int main() {
    try {
        test_vector(user::physics_t{ true, 2.0, 3.f, 42 });
        std::cout << "Success !\n";
    }
    catch (std::exception const& e) {
        std::cout << "Error : " << e.what() << '\n';
        return 1;
    }
}
