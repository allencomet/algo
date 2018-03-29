#include <iostream>
#include <algorithm>
#include <limits>
#include <bitset>

#include "util/util.h"


namespace heap{

void random(uint32 seed,int32 arr[],int32 num,int l,int r){
    srand(seed);
    for(int32 i = 0; i < num; ++i)
        arr[i] = rand()%(r-l+1)+l;
}

template<typename T>
class MaxHeap{
 public:
 MaxHeap(int capacity = 100)
    :_count(0),_capacity(capacity){
    _data = new T[_capacity+1];
 }

//Heapify的算法复杂度
//将n个元素逐个插入到一个空堆中，算法复杂度是O(nlogn)
//heapify的过程，算法复杂度为O(n)
 MaxHeap(T arr[],int n,int capacity = 100)
    :_count(n){
    if(n > 100) capacity = n*3/2;
    _capacity = capacity; 
     _data = new T[_capacity+1];

    for(int i = 1; i <= _count; ++i)
        _data[i] = arr[i-1];

    for(int i = _count/2; i >= 1; --i) 
        shiftDown(i);
 }

 ~MaxHeap(){
     if(NULL != _data){
         delete[] _data;
         _data = NULL;
     }
 }

 int capacity() const {
     return _capacity;
 }

 int size() const {
     return _count;
 }

 bool empty(){
     return _count == 0;
 }

 void insert(T item){
     assert(_count + 1 < _capacity);

     _data[++_count] = item;//尾部添加(0索引不适用)
    shiftUp(_count);
 }

 T popMax(){
     if(this->empty()) return T();
     T item = _data[1];
     _data[1] = _data[_count--];
     shiftDown(1);
     return item;
 }

 void print(){
     for(int i = 1; i <= _count; ++i){
         std::cout << _data[i] << " ";
     }
     std::cout << std::endl;
 }

 private:
    void shiftUp(int index){
        while(index/2 >= 1 && _data[index/2] < _data[index]){
            std::swap(_data[index],_data[index/2]);
            index /= 2;
        }
    }

    void shiftDown(int index){
        int k;
        while(index*2 <= _count){
            k = index*2;
            if(k+1 <= _count && _data[k] < _data[k+1]) ++k;
            if(_data[index] < _data[k]){
                std::swap(_data[index],_data[k]);
                index = k;
            }else{
                break;
            }
        }
    }

    void dynamicAdjustMem(int n){

    }

    T *_data;    //使用动态分配空间
    int _count;     //当前数量
    int _capacity;  //允许最大容量
};

/*
Name: 最大索引堆
作用: （1）交换的时候从数据的交换调整到对索引的交换；（2）每个元素可以根据索引快速取到
原理：比较的是_data[i]的值，但是交换的是_indexes[j]的值

反向查找索引原理：

现有_data数组里入局如下：
下标：0  1  2  3  4  5
数据：21 20 56 78 43 47  ==》 _data[0]==21  _data[1]==20 _data[2]==56 _data[3]==78 _data[4]==43 _data[5]==47

这些数据构造成最大堆如下所示：
                   78
            56           43
        47     20     21

不移动_data的数据，只移动索引，则将堆中的数据换成对应的下标：
                   3
            2           4
        5     1     0
此时堆的组成为：3 2 4 5 1 0 
==>实际堆的数据构成为：_data[3] _data[2] _data[4] _data[5] _data[1] _data[0]

将这些表示成数据结构存起来就是：
_indexes[0]==3 _indexes[1]==2 _indexes[2]==4 _indexes[3]==5 _indexes[4]==1 _indexes[5]==0
那么将_indexes和_data关系表示一下即为：
_data[_indexes[0]] _data[_indexes[1]] _data[_indexes[2]] _data[_indexes[3]] _data[_indexes[4]] _data[_indexes[5]]

所以以上所有操作都是对_indexes进行操作，只不过取数据的时候从_data中取


假设堆中已有_count(=6，同上演示数据)个数据
（1）插入一个元素newItem
首先实际数组末尾新增一个数_data[_count]=newItem
那么同时维护_indexes数组：_indexes[_count] 表示在堆中最后的一个位置进行一次设置:
原来堆组成：3 2 4 5 1 0 
新增后：3 2 4 5 1 0 6  ==》 这个6是_data中新增数据的索引
这个时候索引6对应的_data[6]在这个堆中可能不满足最大堆的原则，所以需要对这个新增的元素进行shiftUp操作

那么进行shiftUp(_count++);  //注意从0开始，_count可以直接表示新增数据的位置

//原理：只要有父节点且父节点比自己小就交换，交换的时候是_indexes，因为堆的实际构成是这个索引
void shiftUp(int k){
    int index = 0;
    while((k-1)/2 >=0){
        x = (k-1)/2;
        index = _indexes[x];
        if(_data[k] >= _data[index]) break;
        std::swap(_indexes[k],_indexes[x]);//实际交换的事索引的位置
        k = x;
    }
}

所以插入的操作为：
void insert(T item){
    assert(_count+1 < _capacity);
    _data[_count] = item;
    _indexes[_count] = _count;
    shiftUp(_count);
    ++_count;
}

(2)取出最大元素
前提还是基于上述6个元素的最大堆中取出首元素：
原来堆组成：3 2 4 5 1 0 
取出一个元素之后：2 4 5 1 0 ==> 0 2 4 5 1 0 即将最后一个元素放到第一个元素位置,然后再进行shiftDown操作

T popMax(){
    assert(_count > 0);
    T temp = _data[_indexes[0]];
    std::swap(_indexes[0],_indexes[_count-1]);
    --_count;
    shiftDown(0);
    return temp;
}

//原理：有子节点且比子节点最大值小即交换
void shiftDown(int k){
    int x = 0;
    while(k*2+1 < _count){
        x = k*2+1;
        if(x+1 < _count && _data[_indexes[x]] < _data[_indexes[x+1]]) ++x;
        if(_data[_indexes[k]] >= _data[_indexes[x]]) break;
        std::swap(_indexes[k],_indexes[x]);
        k = x;
    }
}


(3)修改指定索引的数据
首先将_data[index] = newItem;
然后再将这个所以对应的_indexes中那个value是index的数据进行一下shiftUp再进行一下shiftDown

void change(int index,T newItem){
    _data[index] = newItem;

    //找到这个index实际对应再堆中的哪个下标
    for(int i =0; i < _count; ++i){//这个过程的时间复杂度为O(n)级别，效率较低，如果我们为_indexes也建立索引则很快
        if(_indexes[i] == index){
            shiftUp(i);
            shiftDown(i);
        }
    }
}

===========================================
根据这个change操作我们根据反向查找法去增加这个堆的性能
我们对这个堆的_indexes建立反向索引，即_indexes中的[key,value]就是_reverse[value,key]
则有如下关系：
_indexes[key] == value : _reverse[value] == key 
==》 _indexes[_reverse[value]] = value
==》 _reverse[_indexes[key]] == key
所以我们同时要维护这个反向索引数组：
(1)插入的时候
void insert(T item){
    assert(_count+1 <= _capacity)
    _data[_count] = item;
    _indexes[_count] = _count;
    _reverse[_count] = _count;
    shiftUp(_count);
    ++_count;
}

void shiftUp(int k){
    int x = 0;
    while((k-1)/2 >= 0){
        x = (k-1)/2;
        if(_data[_indexes[k]] >= _data[_indexes[x]]) break;
        std::swap(_indexes[k],_indexes[x]);//交换当前节点与其父节点的_indexes中的值
        _reverse[_indexes[k]] = k;
        _reverse[_indexes[x]] = x;
        k = x;
    }
}

(2)取出的时候
T popMax(){
    assert(_count > 0);
    T temp = _data[_indexes[0]];
    std::swap(_data[_indexes[0]],_data[_indexes[_count-1]]);
    _reverse[_indexes[0]] = 0;
    _reverse[_indexes[_count-1]] = -1;
    --_count;
    shiftDown(0);
    return temp;
}

void shiftDown(int k){
    int x = 0;
    while(k*2+1 < _count){
        x = k*2+1;
        if(x+1 < _count && _data[_indexes[x]] < _data[_indexes[x+1]]) ++x;
        if(_data[_indexes[k]] >= _data[_indexes[x]]) break;
        std::swap(_indexes[k],_indexes[x]);
        _reverse[_indexes[k]] = k;
        _reverse[_indexes[x]] = x;
        k = x;
    }
}

(3)change的时候
void change(int index,T newItem){
    _data[index] = newItem;
    int j = _indexes[index];//一次命中
    shiftUp(j);
    shiftDown(j);
}


*/
template<typename T>
class IndexMaxHeap{
public:
    IndexMaxHeap(int capacity):_capacity(capacity){
        _data = new T[_capacity];
        _indexes = new int[_capacity];
    }
    IndexMaxHeap(T arr[], int n,int capacity = 100){
        _capacity = capacity;
        if(n >= capacity) _capacity = n*3/2;
        _data = new T[_capacity];
        _indexes = new int[_capacity];
        _rev_indexes = new int[_capacity];
        for(int i = 0; i < n; ++i){
            _data[i] = arr[i];
            _indexes[i] = i;
            _rev_indexes[i] = i;
        }
        _count = n;

        //进行heapify
        for(int i = (n-1)/2; i >= 0; --i) shiftDown(i);
    }
    ~IndexMaxHeap(){
        delete[] _data;
        _data = NULL;
        delete [] _indexes;
         _indexes = NULL;
         delete [] _rev_indexes;
         _rev_indexes = NULL;
    }


    int size() const {
        return _count;
    }

    bool empty() const {
        return _count == 0;
    }

    void insert(T item){
        dynamicAdjustMem(_count+1);
        _data[_count] = item;
        _indexes[_count] = _count;
        _rev_indexes[_count] = _count;
        shiftUp(_count);
        ++_count;
    }

    T popMax(){
        assert(_count>0);
        T item = _data[_indexes[0]];
        std::swap(_indexes[_count-1],_indexes[0]);
        _rev_indexes[_indexes[0]] = 0;
        _rev_indexes[_indexes[_count-1]] = 0;
        --_count;
        shiftDown(0);
        return item;
    }

    int popMaxIndex(){
        assert(_count>0);
        int ret = _indexes[0];
        std::swap(_indexes[_count-1],_indexes[0]);
        _rev_indexes[_indexes[0]] = 0;
        _rev_indexes[_indexes[_count-1]] = -1;//最后这个元素不存在了
        --_count;
        shiftDown(0);
        return ret;
    }

    T getItem(int index){
        assert(contain(index));
        return _data[index];
    }

    void change(int index,T newItem){
        assert(contain(index));
        _data[index] = newItem;

    /*
        //遍历查找索引值为index的那个索引列表里面的下标
        for(int i = 0; i < _count; ++i){
            if(_indexes[i] == index){
                shiftUp(i);
                shiftDown(i);
            }
        }
    */

        int j = _rev_indexes[index];
        shiftUp(j);
        shiftDown(j);
    }

    void print(){
        std::cout << "print " << _count << " indexes: " << std::endl;
        for(int i = 0; i < _count; ++i)
            std::cout << _indexes[i] << " ";
        std::cout << std::endl;
        std::cout << "print " << _count << " elements: " << std::endl;
        for(int i = 0; i < _count; ++i)
            std::cout << _data[i] << " ";
        std::cout << std::endl;
    }
private:
    void shiftDown(int k){
        int x = 0;
        while(k*2+1 < _count){
            x = k*2+1;
            if(x+1 < _count && _data[_indexes[x]] < _data[_indexes[x+1]]) ++x;
            if(_data[_indexes[k]] >= _data[_indexes[x]]) break;
            std::swap(_indexes[k],_indexes[x]);
            _rev_indexes[_indexes[k]] = k;
            _rev_indexes[_indexes[x]] = x;
            k = x;
        }
    }

    void shiftUp(int k){
        int x = 0;
        while((k-1)/2 >= 1){
            x = (k-1)/2;
            if(_data[_indexes[k]] < _data[_indexes[x]]) {
                std::swap(_indexes[k],_indexes[x]);
                _rev_indexes[_indexes[k]] = k;                
                _rev_indexes[_indexes[x]] = x;                
                k = x;
            }else{
                break;
            }                
        }
    }

    bool contain(int index){
        if(index >=0 && index < _count)
            return _rev_indexes[index] != -1;
        else
            return false;
    }

    void dynamicAdjustMem(int n){
        if(n >= _capacity){
            _capacity = _capacity*3/2;
            T *temp = new T[_capacity];
            int *temp_indexes = new T[_capacity];
            int *temp_rev_indexes = new T[_capacity];
            for(int i = 0; i < _count; ++i){
                temp[i] = _data[i];
                temp_indexes[i] = _indexes[i];
                temp_rev_indexes[i] = _rev_indexes[i];
            }
            delete[] _data;
            delete[] _indexes;
            delete[] _rev_indexes;
            _data = temp;
            _indexes = temp_indexes;
            _rev_indexes = temp_rev_indexes;
        }
    }


    T *_data;    //使用动态分配空间
    int *_indexes;//在堆中实际分布的数据，这些都是索引
    int *_rev_indexes;//_indexes的反向数组，可以根据索引找到堆中对应的位置
    int _count;     //当前数量
    int _capacity;  //允许最大容量
};


//原地堆排序
template<typename T>
class HeapSort{
public:
    //arr数据元素索引从0开始
    HeapSort(T arr[],int n){
        //(1)进行heapify过程
        for(int i = (n-1)/2; i >= 0; --i) shiftDown(arr,n,i);

        //(2)对整个数组进行原地排序
        for(int i = n-1; i > 0; --i){
            //(1)首尾元素交换
            std::swap(arr[0],arr[i]);
            //(2)对交换到首位置元素重新寻找合适位置,此时元素总量为i，即为n-1个
            shiftDown(arr,i,0);
        }
    }
    ~HeapSort(){}
private:
    /*
        parent(i) = (i-1)/2;
        l-child(i) = i*2 + 1;
        r-child(i) = i*2 + 2;
    */
    //对索引为k的数据向下寻找合适位置
    void shiftDown(T arr[],int n,int k){
        int x = 0;
        while(k*2+1 < n){
            x = k*2+1;
            if(x+1 < n && arr[x] < arr[x+1]) ++x;
            if(arr[k] >= arr[x]) break;
            std::swap(arr[k],arr[x]);
            k = x;
        }
    }
};

}//namespace heap

