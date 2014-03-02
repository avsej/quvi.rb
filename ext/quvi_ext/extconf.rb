# encoding: UTF-8

ENV['RC_ARCHS'] = '' if RUBY_PLATFORM =~ /darwin/

require 'mkmf'

def define(macro, value = nil)
  $defs.push("-D #{[macro.upcase, Shellwords.shellescape(value)].compact.join('=')}")
end

if ENV['DEBUG']
  $CFLAGS << ' -O0 -ggdb3 -pedantic '
end

pkg_config("libquvi-0.9") or abort("libquvi headers not found. http://quvi.sf.net/r/howto/install/")

create_header("quvi_config.h")
create_makefile("quvi_ext")
