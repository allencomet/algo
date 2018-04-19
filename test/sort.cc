#include "../algo/sort.h"

namespace test {

DEF_test(sort) {

	DEF_case(quick_sort_3ways) {
		int32 arr[1000000] = { 0 };
		int32 num = sizeof(arr) / sizeof(arr[0]);
		sort::random(time(NULL), arr, num, 1, num);

#if __cplusplus >= 201103L
		std::function<void(int32[], int32)> quick_sort_fun = std::bind(sort::quick_sort_3ways<int32>, 
			std::placeholders::_1, std::placeholders::_2);
#else
		boost::function<void(int32[], int32)> quick_sort_fun = boost::bind(sort::quick_sort_3ways<int32>, _1, _2);
#endif
		
		sort::test_sort_perfermance("quick_sort", quick_sort_fun, arr, num);

		EXPECT_EQ(sort::isSorted(arr, num), true);
	}
	
}

}

