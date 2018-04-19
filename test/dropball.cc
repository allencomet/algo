#include "../util/util.h"

namespace test {



int dropball(int floor,int brk) {
	int l = 1, r = floor;
	int mid, count = 0;
	while (l <= r) {
		++count;
		mid = l + (r - l) / 2;
		//would break above the brk floor
		if (mid >= brk) {
			COUT << "第" << count << "次摔落：" << "在第[" << mid << "]层摔碎第一只球";
			//摔碎第一只球之后确定了从该楼层网上都会摔碎，所以此时应该从下方区间第一个开始试探
			//不能再采用二分法去试探，因为可能二分之后的中间位置再次摔碎，这时两个球都碎了，就
			//无法确定哪个位置会刚好摔碎了
			int i = l;
			while (i < brk) {
				++count;
				COUT << "第" << count << "次摔落：" << "试验第[" << i << "]层没碎";
				++i;
			}
			++count;
			COUT << "第" << count << "次摔落：" << "在第[" << i << "]层摔碎第二只球";
			COUT << "试验在[" << l << " - " << i-1 << "]层不会摔碎,经过" << count << "次摔落之后推断出在" << i << "层下落会刚好摔碎";
			break;
		} else {
			COUT << "第" << count << "次摔落：" << "试验第[" << mid << "]层没碎";
			l = mid + 1;
		}
	}

	return count;
}

DEF_test(dropball) {
	dropball(20, 6);
	dropball(20, 13);
}

}