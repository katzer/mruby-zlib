assert('Zlib.deflate') do
  assert_true(Zlib.respond_to? :deflate)
end

assert('Zlib.gzip') do
  assert_true(Zlib.respond_to? :gzip)
end

assert('Zlib.inflate') do
  assert_true(Zlib.respond_to? :inflate)
end

assert('Zlib.deflate A*10') do
  assert_equal('A' * 10, Zlib.inflate(Zlib.deflate('A' * 10)))
end

assert('Zlib.gzip A*10') do
  assert_equal('A' * 10, Zlib.inflate(Zlib.gzip('A' * 10)))
end

assert('Zlib.deflate include zero') do
  str = 'abc\0def'
  assert_equal(str, Zlib.inflate(Zlib.deflate(str)))
end

assert('Zlib.gzip include zero') do
  str = 'abc\0def'
  assert_equal(str, Zlib.inflate(Zlib.gzip(str)))
end

assert('Zlib.deflate empty string') do
  str = ''
  str2 = Zlib.inflate(Zlib.deflate(str))
  assert_equal(str, str2)
  assert_equal(0, str2.bytesize)
end

assert('Zlib.gzip empty string') do
  str = ''
  str2 = Zlib.inflate(Zlib.gzip(str))
  assert_equal(str, str2)
  assert_equal(0, str2.bytesize)
end

assert('Zlib.deflate argment type') do
  [nil, 0, Object.new, {}, []].each do |arg|
    assert_raise(TypeError) do
      Zlib.deflate(arg)
    end
  end
end

assert('Zlib.gzip argment type') do
  [nil, 0, Object.new, {}, []].each do |arg|
    assert_raise(TypeError) do
      Zlib.gzip(arg)
    end
  end
end

assert('Zlib.inflate argment type') do
  [nil, 0, Object.new, {}, []].each do |arg|
    assert_raise(TypeError) do
      Zlib.inflate(arg)
    end
  end
end

assert('Zlib.inflate invalid data') do
  assert_raise(RuntimeError) do
    Zlib.inflate("invalid data")
  end
end

assert('vs CRuby Zlib') do
  # CRuby:
  # Zlib::Deflate.deflate("ABCDEFG")
  #  => "x\234strvqus\a\000\a[\001\335"
  assert_equal("ABCDEFG", Zlib.inflate("x\234strvqus\a\000\a[\001\335"))
end
