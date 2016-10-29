import os
import subprocess
import sys 

env = Environment() ;

os_sources = ['src/os/os.cpp']

sources = Glob('src/config/*.cpp') + Glob('src/core/*.cpp') + Glob('src/except/*.cpp') + Glob('src/util/*.cpp') + os_sources

env.SharedLibrary('theoria', sources)

tests = Glob('test/*.cpp') + Glob('test/config/*.cpp') + Glob('test/core/*.cpp') + Glob('test/util/*.cpp')

gcovOut = Glob("src/*/*.gcno") + Glob("src/*/*.gcda") 

def UsesGCov():
    return int(ARGUMENTS.get('gcov', 0))

if  UsesGCov():
    env.Append(CXXFLAGS = ['-fprofile-arcs', '-ftest-coverage'])
    env.Append(LIBS = ['gcov'])
    env.Clean('unittests',  gcovOut)

env.Append(CXXFLAGS='-Wall -std=c++14 -g')
env.Append(CPPPATH=['./include'])

env['ENV']['LD_LIBRARY_PATH'] = ['.', './build', '/home/smangano/dev/theoria/build/']

program1 = env.Program('unittests', tests, LIBPATH=['.', '/usr/local/lib', '/usr/lib/x86_64-linux-gnu'],
LIBS=['theoria','gtest', 'pthread', 'gcov'], CPPPATH=env['CPPPATH'] + ['./test'])
test_alias = Alias('unittests', [program1], program1[0].path, ENV={'LD_LIBRARY_PATH' : ['.', './build',
'/home/smangano/dev/theoria/build/']})

main = Glob('main/*.cpp')
program2 = env.Program('theoria', main, LIBPATH=['.', '/usr/local/lib', '/usr/lib/x86_64-linux-gnu'], LIBS=['theoria', 'gcov'])
theoria_alais = Alias('theoria', [program2], program2[0].path)

#!python
def valgrindTests(env,target,source):
    ret = 0 
    with open(target[0].path, 'w') as out:
        ret = subprocess.call(['valgrind', '--error-exitcode=1', '--tool=memcheck', source[0].children()[0].abspath], stderr=subprocess.STDOUT, stdout=out, env=env['ENV'])
    #Count lines of output    
    count = 0
    with open(target[0].path, 'r') as input:
        for line in input:
            count+=1
    #output last 10 lines or all if error ret
    count2 = 0 
    with open(target[0].path, 'r') as input:
        for line in input:
            count2+=1
            if ret or count2+10 >= count:
                print line,
    sys.exit(ret) 

env.Command("../unittests.valgrind.out",'unittests',valgrindTests, ENV={'LD_LIBRARY_PATH' : './build', 'PATH' : os.environ['PATH']})

#!python
def lcov(env,target,source):
    subprocess.call(['lcov', '-d', 'build', '--capture', '-o', 'build/coverage.info'])
    subprocess.call(['genhtml', 'build/coverage.info', '-o', 'codecoverage'])


if  UsesGCov():
    env.Command("coverage.info",'unittests',lcov)

