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

	DEF_case(BST) {
		ExtractContent ec("bible.txt");

		COUT << "line: " << ec.line();
		COUT << "number Of Words: " << ec.numberOfWords();

		tree::BST<std::string, int> bst;
		const std::vector<std::string>& words = ec.words();
		for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it) {
			int *ret = bst.search(*it);
			if (NULL == ret) {
				bst.insert(*it, 1);
				//COUT << "insert new word: " << *it;
			} else {
				++(*ret);
				//COUT << "word " << *it << " count increase";
			}
		}

		COUT << "BST size: " << bst.size();

		int *value = bst.search("God");
		if (NULL != value) COUT << "God: " << *value;
		else COUT << "cannot find God";
	}
}

}