project('m0dular-framework', 'cpp', 'c', default_options : ['c_std=c11', 'cpp_std=c++17', 'b_ndebug=if-release'])

compile_args = ['-DNUM_THREADS=4', '-Werror', '-Wno-unused-command-line-argument']
linker_args = compile_args

if not get_option('buildtype').startswith('debug')
	compile_args += ['-flto', '-Ofast', '-msse4.2']
  linker_args += ['-flto']
else
  compile_args += ['-g', '-O0']
endif

includes = []
libraries = []
deps = []

cc = meson.get_compiler('cpp')

if meson.is_cross_build()
	add_args = []
  #Cross compile support, assuming all the windows build files have been extracted from a visual studio installation ant placed inside a single directory.
	if host_machine.system() == 'windows'
		cpu_family = host_machine.cpu_family()
		if build_machine.system() != 'windows'
			linker_args += '-fuse-ld=lld'
			add_args = ['-target', 'i386-pc-windows-msvc19.14.26428', '-Wno-msvc-not-found', '-D_CRT_SECURE_NO_WARNINGS', '-D_WINSOCK_DEPRECATED_NO_WARNINGS', '-fms-extensions', '-fms-compatibility', '-fms-compatibility-version=19.14.26428', '-Wno-expansion-to-defined', '-Wno-nonportable-include-path', '-Wno-pragma-pack', '-Wno-ignored-attributes', '-Wno-ignored-pragma-intrinsic', '-Wno-int-to-void-pointer-cast', '-fuse-ld=lld', '-flto', '-Wno-non-virtual-dtor', '-Wno-unused-local-typedef', '-Wno-unknown-pragmas']

			bf_dir = get_option('msvc_dir')
			#includes += join_paths(bf_dir, 'include/clang')
			includes += join_paths(bf_dir, 'include/msvc')
			includes += join_paths(bf_dir, 'include/ucrt')
			includes += join_paths(bf_dir, 'include/um')
			includes += join_paths(bf_dir, 'include/shared')
			includes += join_paths(bf_dir, 'include/winrt')
			libraries += join_paths(bf_dir, 'lib/clang')
			libraries += join_paths(bf_dir, join_paths('lib/msvc', cpu_family))
			libraries += join_paths(bf_dir, join_paths('lib/ucrt', cpu_family))
			libraries += join_paths(bf_dir, join_paths('lib/um', cpu_family))
			libraries += join_paths(bf_dir, join_paths('lib/shared', cpu_family))
			libraries += join_paths(bf_dir, join_paths('lib/winrt', cpu_family))
		else
			add_args = []
			linker_args += '-Wl,-debug:full'
			linker_args += '-Wl,-v'
		endif
		linker_args += '-Bdynamic'
		linker_args += '-lkernel32'
		linker_args += '-lmsvcrt'
	elif host_machine.system() == 'linux'
		linker_args += '-fuse-ld=lld'
		add_args = ['-target', 'x86_64-linux', '-flto']
	endif
	foreach p :  add_args
		compile_args += p
		linker_args += p
	endforeach
endif

if host_machine.system() == 'linux'
  linker_args += '-ldl'
endif

foreach p : libraries
	linker_args += '-L'+p
endforeach

foreach p : includes
	compile_args += '-I'+p
endforeach

#message(compile_args)
#message(linker_args)
#message(libraries)
#message(includes)

thread_dep = dependency('threads')

smtx = executable('shared_mutex_test', files(['tests/shared_mutex.cpp', 'utils/threading.cpp', 'utils/shared_mutex.cpp', 'utils/semaphores.cpp', 'utils/mutex.cpp']), cpp_args : compile_args, link_args : linker_args, dependencies : thread_dep)
mtx = executable('mutex_test', files(['tests/mutex.cpp', 'utils/threading.cpp', 'utils/mutex.cpp', 'utils/semaphores.cpp']), cpp_args : compile_args, link_args : linker_args, dependencies : thread_dep)
tpool = executable('thread_pool_test', files(['tests/thread_pool.cpp', 'utils/threading.cpp', 'utils/mutex.cpp', 'utils/semaphores.cpp']), cpp_args : compile_args, link_args : linker_args, dependencies : thread_dep)
crc = executable('crc_test', files(['tests/crc.cpp', 'utils/crc32.cpp']), cpp_args : compile_args, link_args : linker_args, dependencies : thread_dep)
vectors = executable('vector_test', files(['tests/vector.cpp']), cpp_args : compile_args, link_args : linker_args, dependencies : thread_dep)

test('shared mutex', smtx)
test('mutex', mtx)
test('thread_pool', tpool)
test('crc', crc)
test('vectors', vectors)