MRuby::Gem::Specification.new('mruby-zlib') do |spec|
  spec.license = 'MIT'
  spec.authors = 'Internet Initiative Japan Inc.'

  spec.cc.include_paths << "#{build.root}/src"
  spec.linker.libraries << 'z'
end
