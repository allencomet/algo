import os, sys, time
from glob import glob

ccflags = ['-std=c++0x']

if ARGUMENTS.get('release', '0') == '0':
  ccflags += ['-O2', '-g2', '-Werror', '-Wno-deprecated']
else:
  ccflags += ['-O2', '-g0', '-Wall', ]		

ccdefines = {'_FILE_OFFSET_BITS' : '64', 'DEBUG' : 1, }

env = Environment(CC = 'gcc', 
	LIBS = ['stdc++', 'm', 'pthread', 'dl', 'rt', 'mongoc-1.0', 'bson-1.0'], 
	LIBPATH = ['/usr/lib', '/usr/local/lib', './lib'], 
	LINKFLAGS = ['-Wl,--no-as-needed','-rdynamic'],
	CPPPATH = ['.','/usr/local/include/libbson-1.0','/usr/local/include/libmongoc-1.0'])

env.Append(CPPFLAGS = ccflags)
env.Append(CPPDEFINES = ccdefines)

common_source_files = glob('util/impl/*.cc') + \
			glob('util/json/impl/*.cc') + \
			glob('base/*.cc') + \
	      	glob('base/ccflag/*.cc') + \
	       	glob('base/cclog/*.cc') + \
	       	glob('base/cctest/*.cc')

algo_source_files = glob('algo/impl/*.cc')

db_source_files = glob('db/mongodb/*.cc') + \
			glob('db/redis/*.cc')

test_source_files = glob('test/*.cc')

exe_source_files = ['main.cc'] + common_source_files + algo_source_files + db_source_files + test_source_files

env.Program('exe', exe_source_files)



