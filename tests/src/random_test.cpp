#include "random_test.h"
#include "utility/random.h"

namespace blurringshadow::test::utility
{
    boost::ut::suite& random_test() noexcept
    {
        return boost::ut::get_empty_suite();
    }
}