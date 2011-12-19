srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.check_cfg(package='gtk+-2.0', uselib_store='GTK', args='--cflags --libs')
  conf.check_cfg(package='glib-2.0', args='--cflags --libs', uselib_store='GLIB')
  conf.check_cfg(package='webkitnode-1.0', args='--cflags --libs', uselib_store='WEBKIT')

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.cxxflags = ["-Wall", "-ansi", "-pedantic"]
  obj.target = "nwebkit"
  obj.source = """
               src/linux-gtk.cc
	       src/context_wrap.cc
               """
  obj.uselib = "GTK GLIB WEBKIT"
