project('ccerve', 'cpp', default_options : ['cpp_std=c++17'])
project_description = 'HTTP Server written from scratch in C++'

deps = []
if (host_machine.system() == 'windows')
	message('Platform is Windows! Searching for Winsock2 library (ws2_32)')
	compiler = meson.get_compiler('cpp')
	deps += compiler.find_library('ws2_32', required : true)
endif

subdir('ccerve')

# "srcs" variable is defined in ccerve/src/meson.build
# "incdir" variable is defined in ccerve/meson.build
executable('cerve', srcs, include_directories : incdir, dependencies : deps)
