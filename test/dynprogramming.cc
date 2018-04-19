#include "../util/util.h"

namespace test {

//动态规划：最长公共子序列
/*
X = {x1...xm}	Y = {y1...yn}
Xi = {x1...xi}	Yj = {y1...yj}
			
			(1) 0							i==0 || j==0
C[i][j] =	(2) C[i-1][j-1]+1				i>0 && j>0 && xi==yj
			(3) max{C[i-1][j],C[i][j-1]}	i>0 && j>0 && xi!=yj

			S1={1,3,4,5,6,7,7,8} 和 S2={3,5,7,4,8,6,7,8,2}
*/

#define LCS_MAX(x,y) ((x)>(y)?(x):(y))

/*
递归方式：参数m为数组arr1长度，参数n为数组arr2长度
*/
template<typename T>
int LCS(T arr1[], int m, T arr2[], int n) {
	if (m == 0 || n == 0) return 0;//边界值

	if (arr1[m - 1] == arr2[n - 1])
		return LCS(arr1, m - 1, arr2, n - 1) + 1;//状态转换
	else
		return LCS_MAX(LCS(arr1, m, arr2, n - 1), LCS(arr1, m - 1, arr2, n));//状态转换
}

/*
非递归方式：参数m为数组arr1长度，参数n为数组arr2长度
*/
template<typename T>
int LCSEx(T arr1[], int m, T arr2[], int n) {
	if (m == 0 || n == 0) return 0;

	int count[m][n];

	if (arr1[0] == arr2[0]) count[0][0] = 1;
	else count[0][0] = 0;

	//初始化计算出边界值，即arr2只有一个元素时
	for (int i = 1; i < m; ++i) {
		if (arr1[i] == arr2[0])
			count[i][0] = 1;
		else 
			count[i][0] = count[i-1][0];
	}

	//初始化计算出边界值，即arr1只有一个元素时
	for (int j = 1; j < n; ++j) {
		if (arr2[j] == arr1[0])
			count[0][j] = 1;
		else
			count[0][j] = count[0][j-1];
	}
	
	for (int i = 1; i < m; ++i) {
		for (int j = 1; j < n; ++j) {
			if (arr1[i] == arr2[j]) {
				count[i][j] = count[i - 1][j - 1] + 1;
			} else {
				count[i][j] = LCS_MAX(count[i-1][j], count[i][j-1]);
			}
		}
	}
	return count[m - 1][n - 1];
}

DEF_test(LCS) {
	int arr1[] = { 1,3,4,5,6,7,7,8 };
	int arr2[] = { 3,5,7,4,8,6,7,8,2 };

	EXPECT_EQ(1, LCS(arr1, 2, arr2, 2));
	EXPECT_EQ(2, LCS(arr1, 4, arr2, 2));
	EXPECT_EQ(3, LCS(arr1, 6, arr2, 6));
	EXPECT_EQ(4, LCS(arr1, 7, arr2, 8));
	EXPECT_EQ(5, LCS(arr1, 8, arr2, 9));

	EXPECT_EQ(1, LCSEx(arr1, 2, arr2, 2));
	EXPECT_EQ(2, LCSEx(arr1, 4, arr2, 2));
	EXPECT_EQ(3, LCSEx(arr1, 6, arr2, 6));
	EXPECT_EQ(4, LCSEx(arr1, 7, arr2, 8));
	EXPECT_EQ(5, LCSEx(arr1, 8, arr2, 9));
}

}