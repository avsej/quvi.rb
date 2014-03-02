module Quvi
  module VERSION
    MAJOR = 0
    MINOR = 0
    PATCH = 1

    def self.to_str
      [MAJOR, MINOR, PATCH].join(".")
    end
    class << self; alias to_s to_str; end

    def self.inspect
      to_s.inspect
    end
  end
end
