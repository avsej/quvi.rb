require "bundler/gem_tasks"
require "rake/testtask"

Rake::TestTask.new do |test|
  test.libs << "test" << "."
  test.pattern = 'test/**/test_*.rb'
end

require "rake/extensiontask"

def gemspec
  @clean_gemspec ||= eval(File.read(File.expand_path('quvi.gemspec', File.dirname(__FILE__))))
end

Rake::ExtensionTask.new("quvi_ext", gemspec) do |ext|
  ext.ext_dir = "ext/quvi_ext"
  CLEAN.include "#{ext.lib_dir}/*.#{RbConfig::CONFIG['DLEXT']}"
end
Rake::Task['test'].prerequisites.unshift('compile')

desc 'Start an irb session and load the library.'
task :console => :compile do
  debug_lib = begin
                require 'debugger'
                '-rrubygems -rdebugger'
              rescue LoadError
                begin
                  require 'byebug'
                  '-rrubygems -rbyebug'
                rescue LoadError
                end
              end
  exec "irb #{debug_lib} -I lib -r'quvi'"
end

task :default => :test
