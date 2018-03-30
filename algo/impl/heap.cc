#include "../heap.h"

namespace heap {

void random(uint32 seed, int32 arr[], int32 num, int l, int r) {
	srand(seed);
	for (int32 i = 0; i < num; ++i)
		arr[i] = rand() % (r - l + 1) + l;
}

}
