project('ccerve', 'cpp')
project_description = 'HTTP Server written from scratch in C++'

spdlog_dep = dependency('spdlog', fallback : ['spdlog', 'spdlog_dep'])
subdir('ccerve')

# "srcs" variable is defined in ccerve/src/meson.build
# "incdir" variable is defined in ccerve/meson.build
executable('cerve', srcs, include_directories : incdir, dependencies : spdlog_dep)
