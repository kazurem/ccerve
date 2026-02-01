project('ccerve', 'cpp', default_options : ['cpp_std=c++23'])
project_description = 'HTTP Server written from scratch in C++'

subdir('ccerve')

# "srcs" variable is defined in ccerve/src/meson.build
# "incdir" variable is defined in ccerve/meson.build
executable('cerve', srcs, include_directories : incdir)
