MRuby::Gem::Specification.new('mruby-iizlib') do |spec|
  spec.license = 'MIT'
  spec.authors = 'Internet Initiative Japan'

  spec.cc.include_paths << "#{build.root}/src"
  spec.linker.libraries << 'z'
end