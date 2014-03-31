require "quvi/version"
require "quvi/error"
require "quvi/handle"
require "quvi_ext"

require 'thread'
require 'monitor'

module Quvi
  def self.parse(*urls)
    options = urls.last.is_a?(Hash) ? urls.pop : {}
    res = parse_multiple(urls, options)
    urls.size == 1 ?  res.first : res
  end

  def self.parse_multiple(urls, options = {})
    return [] if urls.empty?
    num_threads = options.fetch(:threads) { 2 }
    num_threads = [num_threads, urls.size].min
    tasks = Queue.new
    tasks.extend(MonitorMixin)
    urls.each do |url|
      tasks.push([:media, url])
      tasks.push([:playlist, url])
    end
    tasks_tbd = tasks.size
    done = tasks.new_cond
    results = Queue.new
    threads = Array.new(num_threads) do
      Thread.new do
        handle = Quvi::Handle.new(options)
        loop do
          type, url = tasks.pop
          begin
            if res = handle.parse(url, options.merge(:type => type))
              results.push([url, res])
            end
          rescue Quvi::Error => ex
            results.push([url, ex])
          ensure
            tasks.synchronize do
              tasks_tbd -= 1
              done.signal
              break if tasks_tbd <= 0
            end
          end
        end
      end
    end
    tasks.synchronize do
      done.wait_while do
        tasks_tbd > 0
      end
    end if tasks_tbd > 0
    res = []
    until results.empty?
      u, r = results.pop
      res[urls.index(u)] = r
    end
    res
  end
end
