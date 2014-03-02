module Quvi
  module VERSION
    MAJOR = 0
    MINOR = 0
    PATCH = 1
    FULL = [MAJOR, MINOR, PATCH].join(".").freeze

    def self.to_str
      FULL
    end

    def self.to_hash
      @version_hash ||= begin
                          require 'quvi_ext'
                          {
                            version: to_s,
                            quvi_version: QUVI_VERSION,
                            quvi_configuration: QUVI_CONFIGURATION,
                            quvi_build_cc_cflags: QUVI_BUILD_CC_CFLAGS,
                            quvi_build_target: QUVI_BUILD_TARGET,
                            quvi_build_time: QUVI_BUILD_TIME
                          }.freeze
                        end
    end
    class << self
      alias to_s to_str
      alias to_h to_hash
    end

    def self.inspect
      to_s.inspect
    end
  end
end
