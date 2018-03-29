#pragma once

#include "./base/base.h"
#include "./boost/lexical_cast.hpp"

/*
 *		��ȫ�ġ����׳��쳣������ֵת����ת��ʧ����ʹ��Ĭ��ֵ
 *		�ַ���ƴ��Ч�ʣ�http://www.cnblogs.com/james6176/p/3222671.html
 *		�����ʹ��Linuxϵͳ������g++��gcc��������������ƴ���ַ�����Ч�ʴӸߵ�������Ϊ��+=��append()��stringstream��sprintf()
 *   
 */

namespace util {

	//���ת��ʧ�ܣ���ʹ��Ĭ��ֵ��������������Ĭ��ֵ
	template <typename L,typename R>
	R safe_lexical_cast(L l,R &ra,const R &rb)
	{
		if (boost::conversion::try_lexical_convert(l , ra))
		{
			return ra;
		}
		else
		{
			return rb;
		}
	}

	//���ת��ʧ�ܣ���ʹ��Ĭ��ֵ���ڶ���������Ĭ��ֵ
	template <typename L,typename R>
	R safe_lexical_cast(L l,R &r)
	{
		R defaulr = r;
		if (boost::conversion::try_lexical_convert(l , r))
		{
			return r;
		}
		else
		{
			return defaulr;
		}
	}
}