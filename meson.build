project(
  'albafetch',
  'c',
  version : '4.0',
  default_options : ['warning_level=3'])


src = [
'src/info.c',
'src/main.c',
'src/queue.c',
'src/utils.c',
]

project_dependencies = [
  dependency('libcurl', method: 'pkg-config'),
]

if host_machine.system() == 'linux'
  project_dependencies += dependency('libpci', method: 'pkg-config')
endif

if host_machine.system() == 'darwin'
  project_dependencies += dependency('appleframeworks', modules : ['foundation', 'iokit'])
  add_languages('objc')
  src += [
    'src/macos_infos.c',
    'src/bsdwrap.c',
    'src/macos_gpu_string.m',
  ]
endif

build_args = [
  '-Wall',
  '-Wextra',
  '-Ofast',
]

executable(
  meson.project_name(),
  src,
  dependencies : project_dependencies,
  install : true,
  c_args : build_args
)

build_args += '-D_DEBUG'
executable(
  'debug',
  src,
  dependencies : project_dependencies,
  install : false,
  c_args : build_args
)

