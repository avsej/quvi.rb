require 'helper'

class TestVersion < Minitest::Test
  def test_it_should_behave_like_a_string
    assert_match /\d+\.\d+\.\d+/, Quvi::VERSION
  end
end
