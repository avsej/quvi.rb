module Quvi
  class Handle
    DEFAULT_USER_AGENT = "Mozilla/5.0".freeze

    attr_accessor :user_agent, :allow_cookies

    def parse(url, options = {})
      online = options.fetch(:online) { true }
      type = options.fetch(:type) { true }
      if supports?(url, :type => type, :online => online)
        res = send("parse_#{type}", url)
        res[:type] = type
        res
      end
    end
  end
 end
