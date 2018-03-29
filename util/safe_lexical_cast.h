#pragma once

#include "./base/base.h"
#include "./boost/lexical_cast.hpp"

/*
 *		安全的、不抛出异常的字面值转换，转换失败则使用默认值
 *		字符串拼接效率：http://www.cnblogs.com/james6176/p/3222671.html
 *		如果是使用Linux系统并且是g++（gcc）编译器，大量拼接字符串的效率从高到低依次为：+=、append()、stringstream、sprintf()
 *   
 */

namespace util {

	//如果转换失败，则使用默认值，第三个参数是默认值
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

	//如果转换失败，则使用默认值，第二个参数是默认值
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