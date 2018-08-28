
/*
    aggregate_vector.hpp
    MIT license (2018)
    Header repository : https://github.com/Dwarfobserver/AggregateVector
    You can join me at sidney.congard@gmail.com
 */

#pragma once

#include <cstring>
#include <cstddef>
#include <utility>
#include <memory>
#include <tuple>


namespace av {

// Holds arrays for each T component in a single allocation.
template <class T, class Allocator = std::allocator<T>>
class vector;

// Iterable object accessed in av::vector<Aggregate> through av::member<Aggregate>.
template <size_t Pos, class Aggregate, class T>
class vector_span;

// Specialized for aggregates so av::vector<T> can be istanciated.
// Specialization of non-template types can be done with the macro
// 'AV_DEFINE_TYPE(type, members...);' in the global namespace.
template <class Aggregate>
struct members {};

// Exemple of specialization (for std::pair).
template <class T1, class T2>
struct members<std::pair<T1, T2>> {
    vector_span<0, std::pair<T1, T2>, T1> first; 
    vector_span<1, std::pair<T1, T2>, T2> second;
};

namespace detail {

    namespace impl {
        template <class T, size_t I>
        using indexed_alias = T;

        template <class T, class Seq>
        struct repeat_tuple {};
        
        template <class T, size_t...Is>
        struct repeat_tuple<T, std::index_sequence<Is...>> {
            using type = std::tuple<indexed_alias<T, Is>...>;
        };
    }
    // Equivalent of std::tuple<T, T, T...N times>.
    template <class T, size_t N>
    using repeat_tuple_t = typename impl::repeat_tuple<T, std::make_index_sequence<N>>::type;

    namespace impl {
        template <size_t I, class...Ts>
        struct get {};
        template <size_t I, class T, class...Ts>
        struct get<I, T, Ts...> {
            using type = typename get<I - 1, Ts...>::type;
        };
        template <class T, class...Ts>
        struct get<0, T, Ts...> {
            using type = T;
        };
    }
    // An empty type used to pass types.
    template <class...Ts>
    struct type_tag {
        template <size_t I>
        using get = typename impl::get<I, Ts...>::type;
        using type = get<0>;
    };

    namespace impl {
        template <class Tuple>
        struct tuple_tag {};
        template <class...Ts>
        struct tuple_tag<std::tuple<Ts...>> {
            using type = type_tag<Ts...>;
        };
    }
    // std::tuple<Ts...> gives type_tag<Ts...>.
    template <class Tuple>
    using tuple_tag = typename impl::tuple_tag<Tuple>::type;

    // Base class of av::vector<T>.
    // Used to retrieve the size by av::vector_span<Offset, T, MemberT> from members<T>.
    template <class T>
    class members_with_size : public members<T> {
        template <size_t, class, class>
        friend class ::av::vector_span;
    protected:
        int size_;
    };

} // ::detail

template <size_t Pos, class Aggregate, class T>
class vector_span {
    template <class, class>
    friend class vector;
    template <class>
    friend struct members;
public:
    using value_type = T;

    T*       data()       noexcept { return ptr_; }
    T const* data() const noexcept { return ptr_; }

    int size() const noexcept;

    T&       operator[](int i)       noexcept { return ptr_[i]; }
    T const& operator[](int i) const noexcept { return ptr_[i]; }

    T&       at(int i)       { at_check(i); return ptr_[i]; }
    T const& at(int i) const { at_check(i); return ptr_[i]; }

    T*       begin()       noexcept { return ptr_; }
    T const* begin() const noexcept { return ptr_; }

    T*       end()       noexcept { return ptr_ + size(); }
    T const* end() const noexcept { return ptr_ + size(); }
private:
    void at_check(int i) const {
        if (i >= size()) throw std::out_of_range
            {"av::vector_span at() out of range"};
    }

    vector_span() = default;
    vector_span(vector_span const&) = default;
    vector_span& operator=(vector_span const&) = default;
    
    vector_span(std::byte* ptr) noexcept :
        ptr_{ reinterpret_cast<T*>(ptr) }
    {}

    T* ptr_;
};

// Template arguments are used to retrieve the size from detail::members_with_size<Aggregate>.
template <size_t Pos, class Aggregate, class T>
int vector_span<Pos, Aggregate, T>::size() const noexcept {
    auto const mem_ptr = reinterpret_cast<members<Aggregate> const*>(this - Pos);
    auto const mws_ptr = static_cast<detail::members_with_size<Aggregate> const*>(mem_ptr);
    return mws_ptr->size_;
}

namespace detail {
    // Aggregate to tuple implementation, only for av::member<T>.

    template <class T>
    auto as_tuple(T& agg, std::integral_constant<int, 1>) {
        auto& [v1] = agg;
        return std::forward_as_tuple(v1);
    }
    template <class T>
    auto as_tuple(T& agg, std::integral_constant<int, 2>) {
        auto& [v1, v2] = agg;
        return std::forward_as_tuple(v1, v2);
    }
    template <class T>
    auto as_tuple(T& agg, std::integral_constant<int, 3>) {
        auto& [v1, v2, v3] = agg;
        return std::forward_as_tuple(v1, v2, v3);
    }
    template <class T>
    auto as_tuple(T& agg, std::integral_constant<int, 4>) {
        auto& [v1, v2, v3, v4] = agg;
        return std::forward_as_tuple(v1, v2, v3, v4);
    }
    template <class T>
    auto as_tuple(T& agg, std::integral_constant<int, 5>) {
        auto& [v1, v2, v3, v4, v5] = agg;
        return std::forward_as_tuple(v1, v2, v3, v4, v5);
    }
    template <class T>
    auto as_tuple(T& agg, std::integral_constant<int, 6>) {
        auto& [v1, v2, v3, v4, v5, v6] = agg;
        return std::forward_as_tuple(v1, v2, v3, v4, v5, v6);
    }
    template <class T>
    auto as_tuple(T& agg, std::integral_constant<int, 7>) {
        auto& [v1, v2, v3, v4, v5, v6, v7] = agg;
        return std::forward_as_tuple(v1, v2, v3, v4, v5, v6, v7);
    }
    template <class T>
    auto as_tuple(T& agg, std::integral_constant<int, 8>) {
        auto& [v1, v2, v3, v4, v5, v6, v7, v8] = agg;
        return std::forward_as_tuple(v1, v2, v3, v4, v5, v6, v7, v8);
    }
    template <class T>
    auto as_tuple(T& agg, std::integral_constant<int, 9>) {
        auto& [v1, v2, v3, v4, v5, v6, v7, v8, v9] = agg;
        return std::forward_as_tuple(v1, v2, v3, v4, v5, v6, v7, v8, v9);
    }
    template <class T>
    auto as_tuple(T& agg, std::integral_constant<int, 10>) {
        auto& [v1, v2, v3, v4, v5, v6, v7, v8, v9, v10] = agg;
        return std::forward_as_tuple(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10);
    }

    // The arity is the number of members of a well-formed av::member<T>.
    template <class Members>
    constexpr int arity_v = sizeof(Members) / sizeof(vector_span<0, vector<char>, char>);

    // Continue the overloads above to increase the max_arity.
    constexpr int max_arity = 10;

    // Converts a well-formed av::member<T> to a tuple with references on each member of the class.
    template <class T>
    auto as_tuple(members<T> const& agg) {
        return as_tuple(agg, std::integral_constant<int, arity_v<members<T>>>{});
    }
    template <class T>
    auto as_tuple(members<T>& agg) {
        return as_tuple(agg, std::integral_constant<int, arity_v<members<T>>>{});
    }

    // Allows to converts any aggregate to a tuple given it's arity.
    template <size_t Arity, class T>
    auto as_tuple(T&& agg) {
        return as_tuple(agg, std::integral_constant<int, Arity>{});
    }

    // for_each loops takes a function object to operate on one or two tuples of references.

    template <class F, size_t...Is, class...Ts>
    constexpr void for_each(std::tuple<Ts&...> const& tuple, F&& f, std::index_sequence<Is...>) {
        (f(std::get<Is>(tuple), type_tag<typename Ts::value_type>{}), ...);
    }
    template <class F, class...Ts>
    constexpr void for_each(std::tuple<Ts&...> const& tuple, F&& f) {
        using seq = std::make_index_sequence<sizeof...(Ts)>;
        detail::for_each(tuple, f, seq{});
    }

    template <class F, size_t...Is, class...Ts1, class...Ts2>
    constexpr void for_each(std::tuple<Ts1&...> const& t1, std::tuple<Ts2&...> const& t2, F&& f, std::index_sequence<Is...>) {
        (f(std::get<Is>(t1), std::get<Is>(t2), type_tag<typename Ts1::value_type>{}), ...);
    }
    template <class F, class...Ts1, class...Ts2>
    constexpr void for_each(std::tuple<Ts1&...> const& t1, std::tuple<Ts2&...> const& t2, F&& f) {
        static_assert(sizeof...(Ts1) == sizeof...(Ts2));
        using seq = std::make_index_sequence<sizeof...(Ts1)>;
        detail::for_each(t1, t2, f, seq{});
    }

} // ::detail

// Stores components of the aggregate T (given by the specialization av:member<T>)
// in successives arrays from an unique continuous allocation.
// It increases the performance when the access patterns are differents for the
// aggregate's members.
// Over-aligned types are not supported.
template <class T, class Allocator>
class vector : public detail::members_with_size<T> {
public:
    // The given allocator is reboud to std::byte to store the different member types.
    using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<std::byte>;

    // The number of T members.
    static constexpr int components_count = detail::arity_v<members<T>>;

    // Constructors.
    vector(Allocator allocator = Allocator{}) noexcept;
    vector(vector&& rhs) noexcept;
    vector(vector const& rhs);

    // Assignments.
    vector& operator=(vector&& rhs) noexcept;
    vector& operator=(vector const& rhs); // TODO Keep array if great enough.

    // Destructor.
    ~vector();

    // Size or capacity modifiers.
    void clear() noexcept;
    void reserve(int capacity);
    void resize(int size);
    void resize(int size, T const& value);
    void shrink_to_fit();

    // Add and remove an element.
    template <class...Ts>
    void emplace_back(Ts&&...components);
    void push_back(T const& value);
    void push_back(T&& value);
    void pop_back();

    // Accessors.
    int  size()     const noexcept { return this->size_; }
    int  capacity() const noexcept { return capacity_; }
    bool empty()    const noexcept { return size() == 0; }

    template <size_t I>
    auto& get_component() noexcept;
    template <size_t I>
    auto const& get_component() const noexcept;
private:
    // Some static asserts on the av::member<T> type.
    static constexpr void check_members();
    static constexpr auto check_members_trigger = (check_members(), 0);

    // Explicit cast to base class.
    members<T>&      base()       noexcept { return *this; }
    members<T>const& base() const noexcept { return *this; }

    detail::members_with_size<T>&       base_with_size()       noexcept { return *this; }
    detail::members_with_size<T> const& base_with_size() const noexcept { return *this; }

    using sequence_type = std::make_index_sequence<components_count>;

    // components_tag = detail::type_tag<Ts...>.
    template <class Tuple>
    struct components_tag_impl;
    template <class...Members>
    struct components_tag_impl<std::tuple<Members&...>> {
        using type = detail::type_tag<typename Members::value_type...>;
    };
    using components_tag = typename components_tag_impl<decltype(
        detail::as_tuple(std::declval<members<T>>())
    )>::type;

    using allocator_traits = std::allocator_traits<allocator_type>;

    // Functions implementations.

    template <class Tuple, size_t...Is>
    void push_back_copy(Tuple const& tuple, std::index_sequence<Is...>);
    template <class Tuple, size_t...Is>
    void push_back_move(Tuple& tuple, std::index_sequence<Is...>);

    template <size_t I, class...Members, class T1, class...Ts>
    void emplace_back_impl(std::tuple<Members&...> const& members, T1&& component, Ts&&...nexts);
    template <size_t I, class...Members>
    void emplace_back_impl(std::tuple<Members&...> const& members);

    // Computes the bytes padding for each component,
    // assuming we start with a 8-bytes aligned address.
    template <size_t I, class...Ints>
    static void update_shift(std::tuple<Ints...>& shifts, int nb, int acc);

    // Creates vector_spans based on the data allocated
    // and the computed shift for each component.
    template <class Tuple, size_t...Is>
    static members<T> create_members(std::byte* ptr, Tuple const& shift, std::index_sequence<Is...>);

    struct alloc_result {
        members<T> new_members;
        int nb_bytes;
    };
    // Allocates unitialized array of 'nb' elements.
    alloc_result allocate(int nb);

    static void copy_array(members<T> const& src, members<T>& dst, int nb);
    static void move_array(members<T>&       src, members<T>& dst, int nb);

    void destroy() noexcept;
    void deallocate() noexcept;

    // Sets the vector fields (size, capacity, ...) according to an empty vector.
    void to_zero() noexcept;

    int capacity_;
    allocator_type allocator_;
    int nb_bytes_;
};

// av::vector implementation.

template <class T, class Allocator>
constexpr void vector<T, Allocator>::check_members() {
    static_assert(!std::is_empty_v<members<T>>,
        "av::members<T> must be specialized to hold "
        "an av::vector_span for each member of T");
    
    static_assert(detail::arity_v<members<T>> <= detail::max_arity,
        "av::members<T> must have less than 'max_arity' members. "
        "This limit can be increased by writing more overloads of 'as_tuple'.");
}

// Constructors.

template <class T, class Allocator>
vector<T, Allocator>::vector(Allocator allocator) noexcept :
    detail::members_with_size<T>{},
    capacity_ { 0 },
    allocator_{ allocator },
    nb_bytes_ { 0 }
{}

template <class T, class Allocator>
vector<T, Allocator>::vector(vector&& rhs) noexcept :
    detail::members_with_size<T>{ rhs.base_with_size() },
    capacity_ { rhs.capacity() },
    allocator_{ rhs.allocator_ },
    nb_bytes_ { rhs.nb_bytes_ }
{
    rhs.to_zero();
}

template <class T, class Allocator>
vector<T, Allocator>::vector(vector const& rhs) :
    detail::members_with_size<T>{ rhs.base_with_size() },
    capacity_ { rhs.size() },
    allocator_{ rhs.allocator_ },
    nb_bytes_ { rhs.nb_bytes_ }
{
    if (rhs.empty()) return;

    auto [new_members, nb_bytes] = allocate(size());
    copy_array(rhs.base(), new_members, size());
    base()    = new_members;
    nb_bytes_ = nb_bytes;
}

// Assignments.

template <class T, class Allocator>
vector<T, Allocator>& vector<T, Allocator>::operator=(vector&& rhs) noexcept {
    destroy();
    deallocate();
    base_with_size() = rhs.base_with_size();
    capacity_  = rhs.capacity();
    allocator_ = rhs.allocator_;
    nb_bytes_  = rhs.nb_bytes_;
    rhs.to_zero();
    return *this;
}
    
template <class T, class Allocator>
vector<T, Allocator>& vector<T, Allocator>::operator=(vector const& rhs) {
    auto const size = rhs.size();
    if (capacity() < size) {
        destroy();
        deallocate();
        auto [new_members, nb_bytes] = allocate(size);
        base()    = new_members;
        nb_bytes_ = nb_bytes;
        capacity_ = size;
    }
    else {
        destroy();
    }
    this->size_ = size;
    copy_array(rhs.base(), base(), size);
    return *this;
}

// Destructor.
template <class T, class Allocator>
vector<T, Allocator>::~vector() {
    destroy();
    deallocate();
}

// Size & capacity modifiers.

template <class T, class Allocator>
void vector<T, Allocator>::clear() noexcept {
    destroy();
    this->size_ = 0;
}

template <class T, class Allocator>
void vector<T, Allocator>::reserve(int capacity) {
    if (capacity <= this->capacity()) return;
    
    auto [new_members, nb_bytes] = allocate(capacity);
    move_array(base(), new_members, size());
    base()    = new_members;
    nb_bytes_ = nb_bytes;
    capacity_ = capacity;
}

template <class T, class Allocator>
void vector<T, Allocator>::resize(int size) {
    if (size <= this->size()) return;

    reserve(size);
    detail::for_each(detail::as_tuple(base()), [this, size] (auto& span, auto tag) {
        using type = typename decltype(tag)::type;
        auto it = span.begin() + this->size();
        auto const end = span.begin() + size;
        for (; it < end; ++it) {
            new (it) type();
        }
    });
    this->size_ = size;
}

template <class T, class Allocator>
void vector<T, Allocator>::resize(int size, T const& value) {
    if (size <= this->size()) return;

    reserve(size);
    auto const tuple = detail::as_tuple<components_count>(value);
    detail::for_each(detail::as_tuple(base()), tuple, [this, size] (auto& span, auto& val, auto tag) {
        using type = typename decltype(tag)::type;
        auto it = span.begin() + this->size();
        auto const end = span.begin() + size;
        while (it < end) {
            new (it) type(val); ++it;
        }
    });
    this->size_ = size;
}

template <class T, class Allocator>
void vector<T, Allocator>::shrink_to_fit() {
    if (size() == capacity()) return;
    reallocate(size());
}

// Add and remove an element.

template <class T, class Allocator>
void vector<T, Allocator>::push_back(T const& value) {
    auto const tuple = detail::as_tuple<components_count>(value);
    push_back_copy(tuple, sequence_type{});
}

template <class T, class Allocator>
void vector<T, Allocator>::push_back(T&& value) {
    auto tuple = detail::as_tuple<components_count>(value);
    push_back_move(tuple, sequence_type{});
}

template <class T, class Allocator>
template <class...Ts>
void vector<T, Allocator>::emplace_back(Ts&&...components) {
    if (size() == capacity()) {
        auto const new_capacity = size() == 0 ? 1 : capacity() * 2;
        reserve(new_capacity);
    }
    emplace_back_impl<0>(detail::as_tuple(base()), std::forward<Ts>(components)...);
    ++this->size_;
}

template <class T, class Allocator>
void vector<T, Allocator>::pop_back() {
    if (empty()) throw std::logic_error{ "pop_back() called on empty av::vector" };
    --this->size_;
    detail::for_each(detail::as_tuple(base()), [this] (auto& span, auto tag) {
        using type = typename decltype(tag)::type;
        span[size()].~type();
    });
}

// Accessors.

template <class T, class Allocator>
template <size_t I>
auto& vector<T, Allocator>::get_component() noexcept {
    static_assert(I < components_count);
    return std::get<I>(detail::as_tuple(base()));
}

template <class T, class Allocator>
template <size_t I>
auto const& vector<T, Allocator>::get_component() const noexcept {
    static_assert(I < components_count);
    return std::get<I>(detail::as_tuple(base()));
}

// Private functions.

template <class T, class Allocator>
void vector<T, Allocator>::copy_array(members<T> const& src, members<T>& dst, int nb) {
    auto const t1 = detail::as_tuple(src);
    auto const t2 = detail::as_tuple(dst);
    detail::for_each(t1, t2, [nb] (auto& span_src, auto& span_dst, auto tag) {
        using type = typename decltype(tag)::type;
        auto it_src = span_src.begin();
        auto it_dst = span_dst.begin();
        auto const end = span_src.end();
        for (; it_src < end; ++it_src, ++it_dst) {
            new (it_dst) type(*it_src);
        }
    });
}

template <class T, class Allocator>
void vector<T, Allocator>::move_array(members<T>& src, members<T>& dst, int nb) {
    auto const t1 = detail::as_tuple(src);
    auto const t2 = detail::as_tuple(dst);
    detail::for_each(t1, t2, [nb] (auto& span_src, auto& span_dst, auto tag) {
        using type = typename decltype(tag)::type;
        auto it_src = span_src.begin();
        auto it_dst = span_dst.begin();
        auto const end = span_src.end();
        for (; it_src < end; ++it_src, ++it_dst) {
            new (it_dst) type(std::move(*it_src));
        }
    });
}

template <class T, class Allocator>
template <class Tuple, size_t...Is>
void vector<T, Allocator>::push_back_copy(Tuple const& tuple, std::index_sequence<Is...>) {
    emplace_back(std::get<Is>(tuple)...);
}
template <class T, class Allocator>
template <class Tuple, size_t...Is>
void vector<T, Allocator>::push_back_move(Tuple& tuple, std::index_sequence<Is...>) {
    emplace_back(std::move(std::get<Is>(tuple))...);
}

template <class T, class Allocator>
template <size_t I, class...Members, class T1, class...Ts>
void vector<T, Allocator>::emplace_back_impl(std::tuple<Members&...> const& tuple, T1&& component, Ts&&...nexts) {
    if constexpr (I < sizeof...(Members)) {
        using type = typename components_tag::template get<I>;
        auto const it = std::get<I>(tuple).ptr_ + size();
        new (it) type(std::forward<T1>(component));
        emplace_back_impl<I + 1>(tuple, std::forward<Ts>(nexts)...);
    }
}
template <class T, class Allocator>
template <size_t I, class...Members>
void vector<T, Allocator>::emplace_back_impl(std::tuple<Members&...> const& tuple) {
    if constexpr (I < sizeof...(Members)) {
        using type = typename components_tag::template get<I>;
        auto const it = std::get<I>(tuple).ptr_ + size();
        new (it) type();
        emplace_back_impl<I + 1>(tuple);
    }
}

template <class T, class Allocator>
template <size_t I, class...Ints>
void vector<T, Allocator>::update_shift(std::tuple<Ints...>& tuple, int nb, int shift) {
    using prev = typename components_tag::template get<I - 1>;
    if constexpr (I == sizeof...(Ints) - 1) {
        std::get<I>(tuple) = shift + nb * sizeof(prev);
    }
    else {
        using type = typename components_tag::template get<I>;
        constexpr auto align = alignof(type) - 1;
        shift += (nb * sizeof(prev) + align) & ~align;
        std::get<I>(tuple) = shift;
        update_shift<I + 1>(tuple, nb, shift);
    }
}

template <class T, class Allocator>
template <class Tuple, size_t...Is>
members<T> vector<T, Allocator>::create_members(std::byte* ptr, Tuple const& shift, std::index_sequence<Is...>) {
    return { (ptr + std::get<Is>(shift))... };
}

template <class T, class Allocator>
typename vector<T, Allocator>::alloc_result
vector<T, Allocator>::allocate(int nb) {
    constexpr int arity = detail::arity_v<members<T>>;
    auto shift = detail::repeat_tuple_t<int, arity + 1>{};
    update_shift<1>(shift, nb, 0);

    auto const nb_bytes = std::get<arity>(shift);
    auto const ptr = allocator_traits::allocate(allocator_, nb_bytes);
    return { create_members(ptr, shift, sequence_type{}), nb_bytes };
}

template <class T, class Allocator>
void vector<T, Allocator>::destroy() noexcept {
    detail::for_each(detail::as_tuple(base()), [] (auto& span, auto tag) {
        using type = typename decltype(tag)::type;
        for (auto& val : span) val.~type();
    });
}

template <class T, class Allocator>
void vector<T, Allocator>::deallocate() noexcept {
    if (capacity() == 0) return;
    auto const data = reinterpret_cast<std::byte*>(get_component<0>().ptr_);
    allocator_traits::deallocate(allocator_, data, nb_bytes_);
}

template <class T, class Allocator>
void vector<T, Allocator>::to_zero() noexcept {
    base_with_size() = {};
    capacity_ = 0;
    nb_bytes_ = 0;
}

} // namespace av

// Private macros (mainly for AV_PP_MAP, to implement AV_DEFINE_TYPE).

#define AV_PP_EMPTY
#define AV_PP_EMPTY_ARGS(...)

#define AV_PP_EVAL0(...) __VA_ARGS__
#define AV_PP_EVAL1(...) AV_PP_EVAL0 (AV_PP_EVAL0 (AV_PP_EVAL0 (__VA_ARGS__)))
#define AV_PP_EVAL2(...) AV_PP_EVAL1 (AV_PP_EVAL1 (AV_PP_EVAL1 (__VA_ARGS__)))
#define AV_PP_EVAL3(...) AV_PP_EVAL2 (AV_PP_EVAL2 (AV_PP_EVAL2 (__VA_ARGS__)))
#define AV_PP_EVAL4(...) AV_PP_EVAL3 (AV_PP_EVAL3 (AV_PP_EVAL3 (__VA_ARGS__)))
#define AV_PP_EVAL(...)  AV_PP_EVAL4 (AV_PP_EVAL4 (AV_PP_EVAL4 (__VA_ARGS__)))

#define AV_PP_MAP_GET_END() 0, AV_PP_EMPTY_ARGS

#define AV_PP_MAP_NEXT0(item, next, ...) next AV_PP_EMPTY
#define AV_PP_MAP_NEXT1(item, next) AV_PP_EVAL0(AV_PP_MAP_NEXT0 (item, next, 0))
#define AV_PP_MAP_NEXT(item, next)  AV_PP_MAP_NEXT1 (AV_PP_MAP_GET_END item, next)

#define AV_PP_MAP0(f, n, t, x, peek, ...) f(n, t, x) AV_PP_MAP_NEXT (peek, AV_PP_MAP1) (f, n+1, t, peek, __VA_ARGS__)
#define AV_PP_MAP1(f, n, t, x, peek, ...) f(n, t, x) AV_PP_MAP_NEXT (peek, AV_PP_MAP0) (f, n+1, t, peek, __VA_ARGS__)
#define AV_PP_MAP(f, t, ...) AV_PP_EVAL (AV_PP_MAP1 (f, 0, t, __VA_ARGS__, (), 0))

#define AV_PP_MEMBER(nb, type, name) \
    vector_span<nb, type, decltype(std::declval<type>().name)> name;

// Shortcut to specialize av::member<my_type>, by listing all the members
// in their declaration order. It must be used in the global namespace.
// Usage exemple :
// 
// namespace user {
//     struct person {
//         std::string name;
//         int age;
//     };
// } 
//
// AV_DEFINE_TYPE(user::person, name, age);
//
// This is equivalent to typing :
//
// namespace av {
//     template <>
//     struct members<::user::person> {
//         vector_span<0, ::user::person, std::string> name;
//         vector_span<1, ::user::person, int>         age;
//     };
// }
//
#define AV_DEFINE_TYPE(type, ...) \
namespace av { \
    template <> \
    struct members<::type> { \
        AV_PP_MAP(AV_PP_MEMBER, type, __VA_ARGS__) \
    }; \
} \
struct _av__force_semicolon_
