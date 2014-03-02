# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'quvi/version'

Gem::Specification.new do |spec|
  spec.name          = "quvi"
  spec.version       = Quvi::VERSION
  spec.authors       = ["Sergey Avseyev"]
  spec.email         = ["sergey.avseyev@gmail.com"]
  spec.summary       = %q{The library to parse the media stream properties.}
  spec.description   = %q{This is the wrapper for libquvi library, which allows to extract media info (including URLs) from popular media hostings like youtube.}
  spec.homepage      = "http://avsej.net/quvi"
  spec.license       = "AGPL"

  spec.files         = `git ls-files -z`.split("\x0")
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.extensions    = spec.files.grep(%r{^ext/.*/extconf.rb})
  spec.require_paths = ["lib"]

  spec.add_development_dependency "bundler", "~> 1.5"
  spec.add_development_dependency "rake"
end
