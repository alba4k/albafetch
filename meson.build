project(
  'albafetch',
  'c',
  version : '4.1',
  default_options : ['warning_level=3'],
  license : 'MIT'
)

src = [
  'src/queue.c',
  'src/utils.c',
  'src/config/config.c',
  'src/config/parsing.c',
  'src/info/battery.c',
  'src/info/bios.c',
  'src/info/colors.c',
  'src/info/cpu.c',
  'src/info/date.c',
  'src/info/desktop.c',
  'src/info/gpu.c',
  'src/info/gtk_theme.c',
  'src/info/icon_theme.c',
  'src/info/cursor_theme.c',
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

project_dependencies = []

if host_machine.system() == 'linux'
  project_dependencies += dependency('libpci', method: 'pkg-config')
endif

if host_machine.system() == 'darwin'
  project_dependencies += dependency('appleframeworks', modules : ['Foundation', 'IOKit'])
  add_languages('objc')
  src += [
    'src/macos/macos_infos.c',
    'src/macos/bsdwrap.c',
    'src/macos/macos_gpu_string.m',
  ]
endif

src_debug = src + ['src/debug.c']
src += 'src/main.c'

build_args = [
  '-Wall',
  '-Wextra',
  '-Ofast',
  '-std=gnu99',
]

debug_args = build_args + ['-g']

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
  c_args : debug_args
)

