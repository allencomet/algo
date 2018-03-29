import os, sys, time
from glob import glob

ccflags = []

#which allows you to specify a default value to be used if there is no specification on the command line.
if ARGUMENTS.get('release','1') == '1':
  ccflags += ['-O2', '-g3', '-Werror', '-Wno-deprecated']
else:
  ccflags += ['-O2', '-g0', '-Wall']
	
ccdefines = {'_FILE_OFFSET_BITS' : '64', 'DEBUG' : 1, }

env = Environment(CC = 'gcc', 
        LIBS = ['stdc++', 'm', 'pthread', 'crypto', 'ssl', 'dl', 'rt', 'z'], 
        LIBPATH = ['/usr/lib', '/usr/local/lib', './lib'], 
        LINKFLAGS = ['-Wl,--no-as-needed','-rdynamic'],
        CPPPATH = ['.'])

env.Append(CPPFLAGS = ccflags)
env.Append(CPPDEFINES = ccdefines)

common_source_files = glob('util/*.cc') + \
                glob('util/base/*.cc') + \
                glob('util/base/ccflag/*.cc') + \
                glob('util/base/cclog/*.cc') + \
                glob('util/conf/impl/*.cc') + \
                glob('util/tool/impl/*.cc') + \
                glob('util/xml/impl/*.cc')

#print("common source code list: >>")
#for s in common_source_files:
#       print(os.path.realpath(s))
#print('')


source_files = ['algo.cc'] + common_source_files

print("tp source code list: >>")
for s in source_files:
        print(os.path.realpath(s))
print('')


env.Program('algo', source_files)

