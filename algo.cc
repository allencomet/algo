#include "util/util.h"

#include "heap.h"
#include "sort.h"

void test_heap();
void test_sort();

int main(int argc,char **argv){
    test_heap();
    return 0;
}

void test_heap(){
    int32 arr[10] = {0};
    int32 num = sizeof(arr)/sizeof(arr[0]);
    heap::random(time(NULL),arr,num,1,num);

    heap::IndexMaxHeap<int> indexHeap(arr,num);
    indexHeap.print();
    for(int i=0; i < num; ++i)
        std::cout << indexHeap.popMax() << " ";
    std::cout << std::endl;
        
    std::cout << std::endl;
}

void test_sort(){
    int32 arr[1000000] = {0};

    int32 num = sizeof(arr)/sizeof(arr[0]);
    sort::random(time(NULL),arr,num,1,num);

    boost::function<void(int32[],int32)> quick_sort_fun = boost::bind(sort::quick_sort_3ways<int32>,_1,_2);
    sort::test_sort_perfermance("quick_sort", quick_sort_fun, arr, num);
}