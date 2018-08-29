
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../aggregate_vector.hpp"


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

// Compare size and values of a specific component from both vectors.
template <class Span, class Span2>
void check_vector_integrity(Span const& span, Span2 const& span_copy) {
    REQUIRE(span.size() == span_copy.size());
    auto it      = span.begin();
    auto it_copy = span_copy.begin();
    for (; it != span.end(); ++it, ++it_copy) {
        REQUIRE(*it == *it_copy);
    }
    REQUIRE(it_copy == span_copy.end());
}

// Compare size, capacity and values from both vectors.
template <class T, size_t...Is>
void check_vector_integrity(av::vector<T> const& vec, std::vector<T> const& vec_copy, std::index_sequence<Is...>) {
    REQUIRE(vec.empty()    == static_cast<int>(vec_copy.empty()));
    REQUIRE(vec.size()     == static_cast<int>(vec_copy.size()));
    REQUIRE(vec.capacity() == static_cast<int>(vec_copy.capacity()));
    (check_vector_integrity(
        vec.template get_span<Is>(),
        make_component_view<Is, sizeof...(Is)>(vec_copy)
    ), ...);
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
    #define AV_TEST(n, ...) n.v1.__VA_ARGS__; n.v2.__VA_ARGS__; n.check_integrity()
    #define AV_CHECK(n) n.check_integrity()

    auto i1 = vector_interface<T>{};
    AV_CHECK(i1);
    AV_TEST(i1, reserve(4));
    AV_TEST(i1, resize(1));
    AV_TEST(i1, push_back(value));

    auto const j = i1;
    AV_CHECK(j);

    auto i2 = std::move(i1);
    AV_CHECK(i2);
    AV_TEST(i2, emplace_back());
    AV_TEST(i2, pop_back());
    AV_TEST(i2, clear());

    #undef AV_TEST
    #undef AV_CHECK
}
