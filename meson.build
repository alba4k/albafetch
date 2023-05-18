project(
  'albafetch',
  'c',
  version : '4.0',
  default_options : ['warning_level=3'],
  license : 'MIT'
)


src = [
  'src/main.c',
  'src/info.c',
  'src/queue.c',
  'src/utils.c',
]

src_debug = [
  'src/debug.c',
  'src/info.c',
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
  src_debug += [
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

executable(
  'debug',
  src_debug,
  dependencies : project_dependencies,
  install : false,
  c_args : build_args
)

