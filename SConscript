import os
env = Environment(CXXFLAGS='-Wall -std=c++14 -g -fprofile-arcs -ftest-coverage', CPPPATH=['./include'], LIBS=['gcov'])
env['ENV']['LD_LIBRARY_PATH'] = ['.', './build', '/home/smangano/dev/theoria/build/']

os_sources = ['src/os/os.cpp']

sources = Glob('src/config/*.cpp') + Glob('src/core/*.cpp') + Glob('src/except/*.cpp') + Glob('src/util/*.cpp') + os_sources

env.SharedLibrary('theoria', sources)

tests = Glob('test/*.cpp') + Glob('test/config/*.cpp') + Glob('test/core/*.cpp') + Glob('test/util/*.cpp')

program1 = env.Program('unittests', tests, LIBPATH=['.', '/usr/local/lib', '/usr/lib/x86_64-linux-gnu'], LIBS=['theoria','gtest', 'pthread', 'gcov'])
test_alias = Alias('unittests', [program1], program1[0].path, ENV={'LD_LIBRARY_PATH' : ['.', './build',
'/home/smangano/dev/theoria/build/']})

#AlwaysBuild(test_alias)

main = Glob('main/*.cpp')
program2 = env.Program('theoria', main, LIBPATH=['.', '/usr/local/lib', '/usr/lib/x86_64-linux-gnu'], LIBS=['theoria', 'gcov'])
theoria_alais = Alias('theoria', [program2], program2[0].path)

#!python
def valgrindTests(env,target,source):
    import subprocess
    import sys 
    ret = 0 
    with open(target[0].path, 'w') as out:
        ret = subprocess.call(['valgrind', '--error-exitcode=1', '--tool=memcheck', source[0].children()[0].abspath], stderr=subprocess.STDOUT, stdout=out)
    #Count lines of output    
    count = 0
    with open(target[0].path, 'r') as input:
        for line in input:
            count+=1
    #output last 8 line or all if error ret
    count2 = 0 
    with open(target[0].path, 'r') as input:
        for line in input:
            count2+=1
            if ret or count2+8 > count:
                print line,
    sys.exit(ret) 

env.Command("../unittests.valgrind.out",'unittests',valgrindTests, ENV={'LD_LIBRARY_PATH' : './build', 'PATH' : os.environ['PATH']})

