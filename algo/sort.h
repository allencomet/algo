#pragma once

#include <iostream>
#include <algorithm>
#include <limits>


#include "../util/util.h"

namespace sort{
//8 6 2 3 1 5 7 4

/*
(1)选取最小值为元素1,元素1所在索引为4,故arr[0]与arr[4]swap一下,变为：
    {1} [6] [2] 3 8 5 7 4
(2)above步骤确定了arr[0]为1的元素为已排序数据，那么在剩余未排序数据中选取最小值swap,（6 2 3 8 5 7 4）最小值为2，索引为2,即arr[1]与arr[2]swap：
    {1 2} [6] [3] 8 5 7 4
(3)above步骤确定了（arr[0]、arr[1]）为已排序数据，其余（arr[2]、arr[3]、arr[4]、arr[5]、arr[6]、arr[7]）为未排序数据,
未排序数据中最小元素对应小标为3,则直接将arr[2]与arr[3]swap一下：
    {1 2 3} [6] 8 5 7 [4]
(4)依此类推：
    ｛1 2 3 4｝ [8] [5] 7 6
==》
    ｛1 2 3 4 5｝ [8] [7] 6
==》
    ｛1 2 3 4 5 6｝ [8] [7]
==》
    ｛1 2 3 4 5 6 7｝ 8

    原理：在未排序元素中将未排序的首个元素和所有元素中最小的元素交换

*/
template<typename T>
void selection_sort(T arr[],int n){
    
    for(int i = 0; i < n; ++i){
        
        //将剩余未排序的元素最小值对应索引找出
        int min_index = i;
        for(int j = i; j < n; ++j){
            if(arr[j] < arr[min_index]){
                min_index = j;
            }
        }

        //交换当前位置和找出索引的值
        std::swap(arr[i],arr[min_index]);
        //8 1   ==> 1 6 2 3 8 5 7 4
        //6 2   ==> 1 2 6 3 8 5 7 4
    }
}

template<typename T>
void insertion_sort(T arr[],int n){
    T tmp;
    for(int i = 0; i < n-1; ++i){
        tmp = arr[i+1];
        for(int j = i; j >= 0; --j){
            if (tmp < arr[j]) {
                arr[j+1] = arr[j];
            }else{
                arr[j+1] = tmp;
                break;
            }
        }
    }
}

template<typename T>
void insertion_sort(T arr[],int l,int r){
    T tmp;
    for(int i = l+1; i <= r; ++i){
        tmp = arr[i];
        int j = i;
        for(; j > l && tmp < arr[j-1]; --j)
            arr[j] = arr[j-1];
        arr[j] = tmp;
    }
}

//2  3  6  8      1  4  5  7
//0 ---- n/2-1   n/2 ---- n-1
// j               i
//              k

template<class T>
void _merge(T arr[],int l,int m,int r){
    //预先分配空间
    T aux[r-l+1];

    for(int k = l; k <= r; ++k){
        aux[k-l] = arr[k];
    }

    int i = l, j = m+1;
    for(int k = l; k <= r; ++k){
        if(i > m){
            arr[k] = aux[j-l];
            ++j;
        }else if(j > r){
            arr[k] = aux[i-l];
            ++i;
        }else if(aux[i-l] < aux[j-l]){
            arr[k] = aux[i-l];
            ++i;
        }else{
            arr[k] = aux[j-l];
            ++j;
        }
    }

}

template<class T>
void merge_sort_recursion(T arr[],int l,int r){
    if(r-l <= 15){
        insertion_sort(arr,l,r);
        return ;
    }

    int m = (l+r)/2;
    merge_sort_recursion(arr,l,m);
    merge_sort_recursion(arr,m+1,r);
    if(arr[m] > arr[m+1])
        _merge(arr,l,m,r);
}


template<class T>
void merge_sort(T arr[],int n){
    merge_sort_recursion(arr,0,n-1);
}

template<class T>
void merge_sort_no_recursion(T arr[],int n){
    for(int sz = 1; sz <= n; sz += sz){
        for(int i = 0; i+sz < n; i += sz + sz){
                //对[i ... i+sz-1] [i+sz ... i+sz+sz-1]进行归并排序
                _merge(arr,i,i+sz-1,std::min(i+sz+sz-1,n-1));
            }
    }
}

template<class T>
void merge_sort_no_recursion_ex(T arr[],int n){
    for(int sz = 1; sz <= n; sz += sz){
        if(sz <= 15) {
            for(int i = 0; i+sz < n; i += sz + sz){
                insertion_sort(arr,i,std::min(i+sz+sz-1,n-1));
            }
        }else{
            for(int i = 0; i+sz < n; i += sz + sz){
                //对[i ... i+sz-1] [i+sz ... i+sz+sz-1]进行归并排序
                _merge(arr,i,i+sz-1,std::min(i+sz+sz-1,n-1));
            }
        }
    }
}

template<typename T>
int _partition(T arr[],int l,int r){
    std::swap(arr[l],arr[rand()%(r-l+1)+l]);
    T v = arr[l];//第一个元素作为flag
    //[l]{[l+1] .... [j]} {[j+1] ... [i]} ...[r]
    int j = l;  //j的范围为l+1 --> r，j的初始值是l+1，说明左部分区域开始处于无效状态
    for(int i = l+1; i <= r; ++i){
        if(arr[i] < v){
            std::swap(arr[++j],arr[i]);
        }
    }

    std::swap(arr[l],arr[j]);
    return j;
}

template<typename T>
void _quick_sort(T arr[],int l,int r){
    if(r - l <= 15){
        insertion_sort(arr,l,r);
        return;
    }

    int p = _partition(arr,l,r);
    _quick_sort(arr,l,p-1);
    _quick_sort(arr,p+1,r);
}

template<typename T>
void quick_sort(T arr[],int n){
    srand(time(NULL));
    _quick_sort(arr,0,n-1);
}


template<typename T>
void _quick_sort_3ways(T arr[],int l,int r){
    if(r - l <= 15){
        insertion_sort(arr,l,r);
        return ;
    }

    T v = arr[l];
    int i = l+1, lt = l, gt = r+1;
    while(i < gt){
        if(arr[i] < v){
            std::swap(arr[i++],arr[++lt]);
        }else if(arr[i] == v){
            ++i;
        }else{
            std::swap(arr[i],arr[--gt]);//i不变，下次对交换都i的数据进行判断
        }
    }

    std::swap(arr[l],arr[lt]);
    _quick_sort_3ways(arr,l,lt-1);
    _quick_sort_3ways(arr,gt,r);
}

template<typename T>
void quick_sort_3ways(T arr[],int n){
    _quick_sort_3ways(arr,0,n-1);
}

template<typename T>
bool isSorted(T arr[],int n){
    for(int i = 0; i < n-1; ++i)
        if(arr[i] > arr[i+1]) return false;
    return true;
}

void random(uint32 seed, int32 arr[], int32 num, int l, int r);

template<typename T>
void test_sort_perfermance(const std::string &sort_name,
#if __cplusplus >= 201103L
	const std::function<void(T[], int32)> &fun,
#else
	const boost::function<void(T[], int32)> &fun,
#endif
    T arr[],int32 num){
    sys::timer t;
    fun(arr,num);
    std::cout << sort_name << " sort " << num << " elements takes " << t.ms() << " ms" << std::endl;
    /*
    for(int i = 0; i < num; ++i)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
    */
    
    //assert(isSorted(arr,num));
}

class Student{
    public:
    Student(){}
    Student(const std::string &name,double score)
    :_name(name),_score(score){
    }
    ~Student(){}

    Student(const Student &s){
        _name = s.name();
        _score = s.score();
    }

    double score() const {
        return _score;
    }

    std::string name() const {
        return _name;
    }

    double operator()() const{
        return _score;
    }

    bool operator<(const Student &s){
        return _score != s.score() ? _score < s.score() : _name < s.name();
    }

    bool operator>(const Student &s){
        return _score != s.score() ? _score > s.score() : _name > s.name();
    }

    Student& operator=(const Student &s){
        _name = s.name();
        _score = s.score();
    }

    private:
    std::string _name;
    double _score;
};

}//namespace sort
