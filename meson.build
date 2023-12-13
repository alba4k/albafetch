project(
  'albafetch',
  'c',
  version : '4.0',
  default_options : ['warning_level=3'],
  license : 'MIT'
)


src = [
  'src/main.c',
  'src/queue.c',
  'src/utils.c',
  'src/info/bios.c',
  'src/info/colors.c',
  'src/info/cpu.c',
  'src/info/date.c',
  'src/info/desktop.c',
  'src/info/gpu.c',
  'src/info/gtk_theme.c',
  'src/info/host.c',
  'src/info/hostname.c',
  'src/info/info.h',
  'src/info/kernel.c',
  'src/info/light_colors.c',
  'src/info/local_ip.c',
  'src/info/login_shell.c',
  'src/info/memory.c',
  'src/info/os.c',
  'src/info/packages.c',
  'src/info/public_ip.c',
  'src/info/pwd.c',
  'src/info/shell.c',
  'src/info/term.c',
  'src/info/uptime.c',
  'src/info/user.c',
]

src_debug = [
  'src/debug.c',
  'src/queue.c',
  'src/utils.c',
  'src/info/bios.c',
  'src/info/colors.c',
  'src/info/cpu.c',
  'src/info/date.c',
  'src/info/desktop.c',
  'src/info/gpu.c',
  'src/info/gtk_theme.c',
  'src/info/host.c',
  'src/info/hostname.c',
  'src/info/info.h',
  'src/info/kernel.c',
  'src/info/light_colors.c',
  'src/info/local_ip.c',
  'src/info/login_shell.c',
  'src/info/memory.c',
  'src/info/os.c',
  'src/info/packages.c',
  'src/info/public_ip.c',
  'src/info/pwd.c',
  'src/info/shell.c',
  'src/info/term.c',
  'src/info/uptime.c',
  'src/info/user.c',
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

