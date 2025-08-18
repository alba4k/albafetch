project(
  'albafetch',
  'c',
  version : '4.2.1',
  default_options : ['warning_level=3'],
  license : 'MIT',
  meson_version: '>=1.3.0'
)

src = [
  'src/config/config.c',
  'src/config/parsing.c',
  'src/info/battery.c',
  'src/info/bios.c',
  'src/info/colors.c',
  'src/info/cpu.c',
  'src/info/cursor_theme.c',
  'src/info/date.c',
  'src/info/desktop.c',
  'src/info/gpu.c',
  'src/info/gtk_theme.c',
  'src/info/host.c',
  'src/info/hostname.c',
  'src/info/icon_theme.c',
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
  'src/info/swap.c',
  'src/info/term.c',
  'src/info/uptime.c',
  'src/info/user.c',
  'src/optdeps/glib.c',
  'src/optdeps/libpci.c',
  'src/utils/debug.c',
  'src/utils/queue.c',
  'src/utils/utils.c',
  'src/utils/wrappers.c',
]

build_args = [
  '-Wall',
  '-Wextra',
  '-O3',
  '-ffast-math',
  '-std=gnu99',
]

# DEPENDENCY HANDLING

project_dependencies = []

glib = dependency('glib-2.0', required: false)
if glib.found()
  build_args += '-DGLIB_EXISTS'
  project_dependencies += glib
endif

if host_machine.system() == 'linux'
  project_dependencies += dependency('sqlite3', required: true)

  libpci = dependency('libpci', required: false)
  if libpci.found()
    build_args += '-DLIBPCI_EXISTS'
    project_dependencies += libpci
  endif
elif host_machine.system() == 'darwin'
  project_dependencies += dependency('appleframeworks', modules : ['Foundation', 'IOKit'], required: true)
  add_languages('objc')
  src += [
    'src/macos/macos_infos.c',
    'src/macos/bsdwrap.c',
    'src/macos/macos_gpu_string.m',
  ]
endif

# MAKE THE VERSION PRINTED BY albafetch -h DYNAMIC
git = find_program('git', required : false)
commit = '{unknown}'
if git.found()
  commit_cmd = run_command(git, 'rev-parse', '--short', 'HEAD', check: false)
  if commit_cmd.returncode() == 0
    commit = commit_cmd.stdout().strip()
  endif
endif

config = configuration_data()
config.set('VERSION', '"'+meson.project_version()+'"')
config.set('COMMIT', '"'+commit+'"')
configure_file(configuration: config, output: 'version.h')
build_args += '-DHAVE_VERSION_H'

# BUILD

src += 'src/main.c'
# debug_args = build_args + ['-g']

executable(
  meson.project_name(),
  src,
  dependencies : project_dependencies,
  install : true,
  c_args : build_args,
  include_directories: include_directories('.')
)
