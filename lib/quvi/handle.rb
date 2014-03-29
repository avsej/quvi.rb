module Quvi
  class Handle
    DEFAULT_USER_AGENT = "Mozilla/5.0".freeze

    attr_accessor :user_agent, :allow_cookies
  end
end
