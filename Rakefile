require "bundler/gem_tasks"
require "rake/testtask"

Rake::TestTask.new do |test|
  test.libs << "test" << "."
  test.pattern = 'test/**/test_*.rb'
end

desc 'Start an irb session and load the library.'
task :console do
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
