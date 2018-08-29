
#include "tests_helper.hpp"


namespace user {
    struct physics_t {
        float pos;
        float speed;
        float acc;
        int   id;
    };
}
AV_DEFINE_TYPE(user::physics_t, pos, speed, acc, id);

TEST_CASE("comparisons against std::vector") {
    test_vector(user::physics_t{ true, 2.0, 3.f, 42 });
}
