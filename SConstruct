env = Environment(CXXFLAGS='-std=c++11 -g', CPPPATH=['./include'])
sources = Glob('src/*.cpp') + Glob('src/util/*.cpp')
env.SharedLibrary('theoria', sources)

tests = Glob('test/*.cpp') + Glob('test/util/*.cpp')

program = env.Program('testprog', tests, LIBPATH=['.', '/usr/local/lib', '/usr/lib/x86_64-linux-gnu'], LIBS=['theoria','gtest', 'pthread'])
test_alias = Alias('testprog', [program], program[0].path)

AlwaysBuild(test_alias)

