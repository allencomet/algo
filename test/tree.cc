#include "../util/util.h"

#include "../algo/tree.h"

namespace test {

//提取文件内容
class ExtractContent {
public:
	ExtractContent(const std::string &filename){
		os::file file(filename,"r");
		if (NULL != file.fd()){
			_lines = file.getlines();
			file.seek(0, 0);
			//str::split(file.read(), ",. ;", _words);
			splitWord(_lines, _words);
		}
	}

	~ExtractContent() {
		std::vector<std::string>().swap(_words);
	}

	int line() const {
		return _lines.size();
	}

	int numberOfWords() const {
		return _words.size();
	}

	const std::vector<std::string>& words() const {
		return _words;
	}

	const std::vector<std::string>& lines() const {
		return _lines;
	}
private:
	void splitWord(const std::vector<std::string> &lines,std::vector<std::string> &words){
		//std::vector<std::string> lineWords;
		for (std::vector<std::string>::const_iterator it = lines.begin();
			it != lines.end(); ++it){
			//str::split(*it, ' ', lineWords);
			//std::copy(lineWords.begin(), lineWords.end(), std::back_inserter(words));
			str::split(*it, isalpha, false, _words);
		}
	}

	std::vector<std::string> _words;//词汇
	std::vector<std::string> _lines;//每行内容

	DISALLOW_COPY_AND_ASSIGN(ExtractContent);
};

DEF_test(tree) {
	DEF_case(binary_search) {
		int arr[] = { 0,1,2,3,4,5,6,7,8,9 };
		int num = sizeof(arr) / sizeof(int);

		EXPECT_EQ(tree::BinarySearch(arr, num, 0), true);
		EXPECT_EQ(tree::BinarySearch(arr, num, 1), true);
		EXPECT_EQ(tree::BinarySearch(arr, num, 2), true);
		EXPECT_EQ(tree::BinarySearch(arr, num, 3), true);
		EXPECT_EQ(tree::BinarySearch(arr, num, 4), true);
		EXPECT_EQ(tree::BinarySearch(arr, num, 5), true);
		EXPECT_EQ(tree::BinarySearch(arr, num, 11), false);
		EXPECT_EQ(tree::BinarySearch(arr, num, 12), false);
		EXPECT_EQ(tree::BinarySearch(arr, num, 13), false);



		EXPECT_EQ(tree::BinarySearchRecursive(arr, num, 0), true);
		EXPECT_EQ(tree::BinarySearchRecursive(arr, num, 1), true);
		EXPECT_EQ(tree::BinarySearchRecursive(arr, num, 2), true);
		EXPECT_EQ(tree::BinarySearchRecursive(arr, num, 3), true);
		EXPECT_EQ(tree::BinarySearchRecursive(arr, num, 4), true);
		EXPECT_EQ(tree::BinarySearchRecursive(arr, num, 11), false);
	}

	DEF_case(BST_insert) {
		ExtractContent ec("bible.txt");

		COUT << "line: " << ec.line();
		COUT << "number Of Words: " << ec.numberOfWords();

		tree::BST<std::string, int> bst;
		const std::vector<std::string>& words = ec.words();
		for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it) {
			int *ret = bst.search(*it);
			if (NULL == ret) {
				//bst.insert(*it, 1);
				bst.insert_norecursive(*it, 1);
			} else {
				++(*ret);
			}
		}
		EXPECT_EQ(ec.line(), 17255);
		EXPECT_EQ(ec.numberOfWords(), 431180);
		EXPECT_EQ(bst.size(), 10070);
		EXPECT_EQ(*bst.search("God"), 2252);

		//COUT << "BST size: " << bst.size();
		/*int *value = bst.search("God");
		if (NULL != value) COUT << "God: " << *value;
		else COUT << "cannot find God";*/
	}

	DEF_case(BST_traverse) {
		ExtractContent ec("bible.txt");
		tree::BST<std::string, int> bst;
		const std::vector<std::string>& words = ec.words();
		int i = 0;
		for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it) {
			if (i++ < 10){
				int *ret = bst.search(*it);
				if (NULL == ret) {
					//bst.insert(*it, 1);
					bst.insert_norecursive(*it, 1);
				} else {
					++(*ret);
				}
			} else {
				break;
			}
		}

		bst.preOrder();
		bst.inOrder();
		bst.postOrder();
		bst.levelOrder();

		std::pair<std::string, int> retMin = bst.minimum();
		COUT << "minimum: key: " << retMin.first << ",value: " << retMin.second;
		std::pair<std::string, int> retMax = bst.maximum();
		COUT << "maximum: key: " << retMax.first << ",value: " << retMax.second;

		//bst.delMinRecursive();
		//bst.delMinRecursive();
		bst.del("and");
		bst.preOrder();
		bst.del("God");
		bst.preOrder();
		//std::pair<std::string, int> retMin2 = bst.minimum();
		//COUT << "minimum: key: [" << retMin2.first << "],value: [" << retMin2.second << "]";
		

		//bst.delMaxRecursive();
		//bst.delMaxRecursive();
		//bst.preOrder();
	}


}

}