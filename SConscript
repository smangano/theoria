env = Environment(CXXFLAGS='-Wall -std=c++14 -g -fprofile-arcs -ftest-coverage', CPPPATH=['./include'], LIBS=['gcov'])
sources = Glob('src/config/*.cpp') + Glob('src/core/*.cpp') + Glob('src/except/*.cpp') + Glob('src/util/*.cpp') 
env.SharedLibrary('theoria', sources)

tests = Glob('test/*.cpp') + Glob('test/config/*.cpp') + Glob('test/core/*.cpp') + Glob('test/util/*.cpp')

program1 = env.Program('unittests', tests, LIBPATH=['.', '/usr/local/lib', '/usr/lib/x86_64-linux-gnu'], LIBS=['theoria','gtest', 'pthread', 'gcov'])
test_alias = Alias('unittests', [program1], program1[0].path)

AlwaysBuild(test_alias)

main = Glob('main/*.cpp')
program2 = env.Program('theoria', main, LIBPATH=['.', '/usr/local/lib', '/usr/lib/x86_64-linux-gnu'], LIBS=['theoria', 'gcov'])
theoria_alais = Alias('theoria', [program2], program2[0].path)

AlwaysBuild(theoria_alais)


