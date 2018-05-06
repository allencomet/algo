PROJECT(exe)

# 限定了 CMake 的版本
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# 设置编译选项
ADD_DEFINITIONS(-std=c++11
				-g2
				-O2 
				-Werror 
				-Wno-deprecated 
				-Wl,--no-as-needed 
				-rdynamic
				-DDEBUG=1 
				-D_FILE_OFFSET_BITS=64
				-DJEMALLOC_NO_DEMANGLE=1
				) 

# AUX_SOURCE_DIRECTORY 指定目录中的源文件名称赋值给变量 DIR_SRCS
# 格式：aux_source_directory(<dir> <variable>)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
AUX_SOURCE_DIRECTORY(./base BASE_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./base/ccflag BASE_FLAG_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./base/cclog BASE_LOG_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./base/cctest BASE_TEST_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./test TEST_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./test/cpp0x TEST_C11_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./test/boost TEST_BOOST_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./test/protobuf TEST_PROTOBUF_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./algo/impl ALGO_IMPL_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./design DESIGN_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./db/mongodb DB_MONGO_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./db/REDIS REDIS_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./util UTIL_DIR_SRCS)
AUX_SOURCE_DIRECTORY(./util/impl UTIL_IMPL_DIR_SRCS)

# 设置头文件搜索路径
INCLUDE_DIRECTORIES(/usr/local/include)
INCLUDE_DIRECTORIES(/usr/local/include/libbson-1.0)
INCLUDE_DIRECTORIES(/usr/local/include/libmongoc-1.0)

# 设置链接的库文件目录
LINK_DIRECTORIES(/usr/local/lib)

# 设置依赖的库文件
LINK_LIBRARIES(stdc++ m pthread dl rt jemalloc mongoc-1.0 bson-1.0)

# ADD_SUBDIRECTORY 指明本项目包含的子目录（用于在每层子目录单独一个CMakeLists.txt文件时生效）
#ADD_SUBDIRECTORY( base )
#ADD_SUBDIRECTORY( util )
#ADD_SUBDIRECTORY( algo )
#ADD_SUBDIRECTORY( design )
#ADD_SUBDIRECTORY( db )
#ADD_SUBDIRECTORY( test )

# ADD_EXECUTABLE 指示变量 DIR_SRCS 等中的源文件需要编译 成一个名称为 exe 的可执行文件
ADD_EXECUTABLE(exe  ${DIR_SRCS} 
                    ${BASE_DIR_SRCS} 
                    ${BASE_FLAG_DIR_SRCS} 
                    ${BASE_LOG_DIR_SRCS}
                    ${BASE_TEST_DIR_SRCS}
                    ${TEST_DIR_SRCS}
					${TEST_C11_DIR_SRCS}
					${TEST_BOOST_DIR_SRCS}
					${TEST_PROTOBUF_DIR_SRCS}
					${ALGO_IMPL_DIR_SRCS}
                    ${DESIGN_DIR_SRCS}
                    ${DB_MONGO_DIR_SRCS}
                    ${REDIS_DIR_SRCS}
                    ${UTIL_DIR_SRCS}
                    ${UTIL_IMPL_DIR_SRCS}
                    )

# TARGET_LINK_LIBRARIES 指明可执行文件 exe 依赖的链接库
TARGET_LINK_LIBRARIES(exe stdc++ m pthread dl rt jemalloc protobuf mongoc-1.0 bson-1.0)