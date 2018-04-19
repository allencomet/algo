#include "../util/util.h"

#include "../algo/heap.h"

namespace test {

DEF_test(heap) {
	const std::vector<std::string> &names = cctest::args();

	DEF_case(index_heap) {
		int32 arr[10000] = { 0 };
		int32 num = sizeof(arr) / sizeof(arr[0]);
		heap::random(time(NULL), arr, num, 1, num);

		int32 arr_ret[10000] = { 0 };
		heap::IndexMaxHeap<int> indexHeap(arr, num);
		//indexHeap.print();
		for (int i = 0; i < num; ++i)
			arr_ret[num-i-1] = indexHeap.popMax();
		EXPECT_EQ(heap::isSorted(arr_ret, num), true);
	}


	DEF_case(sort_heap) {
		int32 arr[10000] = { 0 };
		int32 num = sizeof(arr) / sizeof(arr[0]);
		heap::random(time(NULL), arr, num, 1, num);

		heap::HeapSort<int> sort(arr, num);
		EXPECT_EQ(heap::isSorted(arr, num), true);
	}
}


}//namespace