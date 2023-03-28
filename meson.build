project('albafetch', 'c',
  version : '0.1',
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
  project_dependencies += dependency('appleframeworks', modules : 'foundation', 'iokit')
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
  'albafetch',
  src,
  dependencies : project_dependencies,
  install : true,
  c_args : build_args
)
