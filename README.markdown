# Quvi

Quvi is a wrapper for [libquvi](http://quvi.sf.net)

## Installation

Follow installation instruction [on quvi
site](http://quvi.sf.net/r/howto/install/) or just install
`libquvi-devel` if you are Fedora user.

    $ sudo yum install libquvi-devel

Add this line to your application's Gemfile:

    gem 'quvi'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install quvi

## Usage

Create an handle which basically encapsulates HTTP session

    handle = Quvi::Handle.new(user_agent: "Chrome/33.0.1712.4")

Pass an media URL to `Quvi::Handle#parse_media` method (note how it tags the best available stream)

    handle.parse_media("https://www.youtube.com/watch?v=9PLY4TWo7f0")
    # {:id=>"9PLY4TWo7f0",
    #  :title=>"Big Ruby 2014 - Keynote by Keavy McMinn",
    #  :thumbnail_url=>"https://i1.ytimg.com/vi/9PLY4TWo7f0/default.jpg",
    #  :duration_ms=>1627000,
    #  :streams=>
    #   [{:id=>"hd720_mp4_i22_720p", :best=>true, :video_height=>720, :video_width=>1280, :video_encoding=>"avc1.64001F", :audio_encoding=>"mp4a.40.2", :container=>"mp4", :url=> "http://..."},
    #    {:id=>"medium_mp4_i18_360p", :video_height=>360, :video_width=>640, :video_encoding=>"avc1.42001E", :audio_encoding=>"mp4a.40.2", :container=>"mp4", :url=> "http://..."},
    #    {:id=>"small_flv_i05_240p", :video_height=>240, :video_width=>320, :video_encoding=>"", :audio_encoding=>"", :container=>"flv", :url=> "http://..."},
    #    {:id=>"small_3gpp_i36_240p", :video_height=>240, :video_width=>320, :video_encoding=>"mp4v.20.3", :audio_encoding=>"mp4a.40.2", :container=>"3gpp", :url=> "http://..."},
    #    {:id=>"small_3gpp_i17_144p", :video_height=>144, :video_width=>176, :video_encoding=>"mp4v.20.3", :audio_encoding=>"mp4a.40.2", :container=>"3gpp", :url=> "http://..."}]}

It can parse playlists too with `Quvi::Handle#parse_playlist`

    handle.parse_playlist("https://www.youtube.com/playlist?list=PLE7tQUdRKcyYqVZi0LIbAsfwv_Jr-rrua")
    # {:id=>"PLE7tQUdRKcyYqVZi0LIbAsfwv_Jr-rrua",
    #  :title=>"Cascadia Ruby 2013",
    #  :thumbnail_url=>"https://i.ytimg.com/vi/MkoTmq3I4DM/hqdefault.jpg",
    #  :media=>
    #   [{:url=>"https://www.youtube.com/watch?v=MkoTmq3I4DM&feature=youtube_gdata", :title=> "Cascadia Ruby 2013 Coding and The Mozart Effect by PJ Hagerty", :duration_ms=>1775000},
    #    {:url=>"https://www.youtube.com/watch?v=0y2Sok9ikj8&feature=youtube_gdata", :title=> "Cascadia Ruby 2013 Taking Ruby to the Movies by Jeremy Hinegardner", :duration_ms=>1776000},
    #    {:url=>"https://www.youtube.com/watch?v=euDXpKwtCHY&feature=youtube_gdata", :title=> "Cascadia Ruby 2013 Tell Us Another Story Grandpa: Lessons Learned over 16 Years as a Developer", :duration_ms=>1643000},
    #    {:url=>"https://www.youtube.com/watch?v=FsBgpvDnUSk&feature=youtube_gdata", :title=> "Cascadia Ruby 2013 Ruby Robotics by Josh Adams and Robby Clements", :duration_ms=>1378000},
    #    {:url=>"https://www.youtube.com/watch?v=rpeugzat2o4&feature=youtube_gdata", :title=> "Cascadia Ruby 2013 We All Make Mistakes: Learning from Gaffes in the Ruby Community by Nick Cox", :duration_ms=>1868000},
    #    {:url=>"https://www.youtube.com/watch?v=g4LJNZKyqOw&feature=youtube_gdata", :title=> "Cascadia Ruby 2013 We're sorry, but something went wrong by Aja Hammerly", :duration_ms=>1909000},
    #    {:url=>"https://www.youtube.com/watch?v=x9Kazz5-tDo&feature=youtube_gdata", :title=> "Cascadia Ruby 2013 The hacker's guide to usability testing by Starr Horne", :duration_ms=>1487000},
    #    {:url=>"https://www.youtube.com/watch?v=iSug6CgxWxc&feature=youtube_gdata", :title=> "Cascadia Ruby 2013 Sentiment Analysis using Support Vector Machines in Ruby by Matthew Kirk", :duration_ms=>1518000}]}

Youtube isn't the only site it supports. You can fine full list of them in the [repository](https://github.com/legatvs/libquvi-scripts) or write your own using guides on the [libquvi-scripts site](http://quvi.sourceforge.net/r/dev/contributing/). Also it is quite easy to enumerate all accessible scripts from the ruby:

    handle.each_script do |script|
      p script
    end
    # {:sha1=>"cc2d07c64cb329d9d14209ce464030a2eb152dc6", :filepath=>"/usr/share/libquvi-scripts/0.9/scan/youtube.lua", :type=>"scan"}
    # {:sha1=>"e145e40e56f27805278ae6a36352bab3225d4d77", :filepath=>"/usr/share/libquvi-scripts/0.9/scan/vzaar.lua", :type=>"scan"}
    # {:sha1=>"a6e55279aaee3bbe6b372fc55b8a6f7751e34ace", :filepath=>"/usr/share/libquvi-scripts/0.9/playlist/youtube.lua", :type=>"playlist", :domains=>["youtube.com"]}
    # {:sha1=>"ddc1d5aaa691131d3ef30cf1b9b482d38cacee51", :filepath=>"/usr/share/libquvi-scripts/0.9/playlist/soundcloud.lua", :type=>"playlist", :domains=>["soundcloud.com"]}
    # {:sha1=>"32e0ead26b603ee10f1deba49e2352d14edd4d67", :filepath=>"/usr/share/libquvi-scripts/0.9/media/audioboo.lua", :type=>"media", :domains=>["audioboo.fm"]}
    # {:sha1=>"a0879b74362cfba9dcaebc3ffe16d034368ed60c", :filepath=>"/usr/share/libquvi-scripts/0.9/media/liveleak.lua", :type=>"media", :domains=>["liveleak.com"]}
    # {:sha1=>"a1427a3a019877eafa5f5f5960d086c609ab8d1f", :filepath=>"/usr/share/libquvi-scripts/0.9/media/bikeradar.lua", :type=>"media", :domains=>["bikeradar.com"]}
    # {:sha1=>"7fd0a857cf904d1547c119444e2ce29297accec1", :filepath=>"/usr/share/libquvi-scripts/0.9/media/sapo.lua", :type=>"media", :domains=>["videos.sapo.pt"]}
    # {:sha1=>"b935fbe2881e0869028741f630fb7471413aeffe", :filepath=>"/usr/share/libquvi-scripts/0.9/media/sevenload.lua", :type=>"media", :domains=>["sevenload.com"]}
    # {:sha1=>"60a7e893e8aa5f71ed1cc5e4a4fb4f89992f85e5", :filepath=>"/usr/share/libquvi-scripts/0.9/media/ina.lua", :type=>"media", :domains=>["ina.fr"]}
    # {:sha1=>"452aa94dc655090d6c30929335bf5ad73d29e92f", :filepath=>"/usr/share/libquvi-scripts/0.9/media/metacafe.lua", :type=>"media", :domains=>["metacafe.com"]}
    # {:sha1=>"2db42ea01ff5700210e814806712c4892a61bda0", :filepath=>"/usr/share/libquvi-scripts/0.9/media/dailymotion.lua", :type=>"media", :domains=>["dailymotion.com"]}
    # {:sha1=>"cfed259fa8f679ca5ac49f6d34144128df716939", :filepath=>"/usr/share/libquvi-scripts/0.9/media/ardmediathek.lua", :type=>"media", :domains=>["ardmediathek.de"]}
    # {:sha1=>"e1ba6e629a11518c3548c69f3723118a7ac1062d", :filepath=>"/usr/share/libquvi-scripts/0.9/media/clipfish.lua", :type=>"media", :domains=>["clipfish.de"]}
    # {:sha1=>"1853a56b0bd5a18d47f967e875b55c6aa3a7dbb7", :filepath=>"/usr/share/libquvi-scripts/0.9/media/101greatgoals.lua", :type=>"media", :domains=>["101greatgoals.com"]}
    # {:sha1=>"363c01107cd71f360c3f8723ecef7bfba2ab3d73", :filepath=>"/usr/share/libquvi-scripts/0.9/media/gaskrank.lua", :type=>"media", :domains=>["gaskrank.tv"]}
    # {:sha1=>"911433da47662580548e93f0529babcce0863853", :filepath=>"/usr/share/libquvi-scripts/0.9/media/theonion.lua", :type=>"media", :domains=>["theonion.com"]}
    # {:sha1=>"0ae3b44c7b01ae3c7e2e7f4a16cd0dcec79f567a", :filepath=>"/usr/share/libquvi-scripts/0.9/media/youtube.lua", :type=>"media", :domains=>["youtube.com"]}
    # {:sha1=>"69974caa7b3fcdb74b6bd791a6edb610b3dfe4ee", :filepath=>"/usr/share/libquvi-scripts/0.9/media/majestyc.lua", :type=>"media", :domains=>["tube.majestyc.net"]}
    # {:sha1=>"a785a669d1bd2c1c253b64fb1f20937e26ef3609", :filepath=>"/usr/share/libquvi-scripts/0.9/media/videobash.lua", :type=>"media", :domains=>["videobash.com"]}
    # {:sha1=>"c3c0e34cb0191570be0ba4cd1499fd2afa44f165", :filepath=>"/usr/share/libquvi-scripts/0.9/media/vzaar.lua", :type=>"media", :domains=>["vzaar.com"]}
    # {:sha1=>"74a632c1a25807b5b094a1c90d614ec865af482d", :filepath=>"/usr/share/libquvi-scripts/0.9/media/spiegel.lua", :type=>"media", :domains=>["spiegel.de"]}
    # {:sha1=>"3720f0611d54ccd665d13603896de9aa903f72f0", :filepath=>"/usr/share/libquvi-scripts/0.9/media/videa.lua", :type=>"media", :domains=>["videa.hu"]}
    # {:sha1=>"d9268d6b51da1bb23932e79c5bb4a1a15c3588c4", :filepath=>"/usr/share/libquvi-scripts/0.9/media/ted.lua", :type=>"media", :domains=>["ted.com"]}
    # {:sha1=>"7aebe106d954b9317f5f7b4a365dacc50933610a", :filepath=>"/usr/share/libquvi-scripts/0.9/media/soundcloud.lua", :type=>"media", :domains=>["soundcloud.com"]}
    # {:sha1=>"a88735d1e0cc3d61ce45af300008ad97f49b85d3", :filepath=>"/usr/share/libquvi-scripts/0.9/media/tvlux.lua", :type=>"media", :domains=>["tvlux.be"]}
    # {:sha1=>"419af1632ffd971505f61bc8d04945a271bb45d6", :filepath=>"/usr/share/libquvi-scripts/0.9/media/1tvru.lua", :type=>"media", :domains=>["1tv.ru"]}
    # {:sha1=>"d3a8db790e9f4e2b55bd0cf88543d8d38e873d4a", :filepath=>"/usr/share/libquvi-scripts/0.9/media/arte.lua", :type=>"media", :domains=>["arte.tv"]}
    # {:sha1=>"73bcf38c7cb2927a0b8adf29a7b750bc4b21a733", :filepath=>"/usr/share/libquvi-scripts/0.9/media/funnyordie.lua", :type=>"media", :domains=>["funnyordie.com"]}
    # {:sha1=>"c027be9b93e362e451ad54adef2d14c6d4e458af", :filepath=>"/usr/share/libquvi-scripts/0.9/media/tapuz.lua", :type=>"media", :domains=>["flix.tapuz.co.il"]}
    # {:sha1=>"3505d6777a8e03ea3bcf01cf113e2c8c9eeec1fd", :filepath=>"/usr/share/libquvi-scripts/0.9/media/cbsnews.lua", :type=>"media", :domains=>["cbsnews.com"]}
    # {:sha1=>"780399daec55e457c766cde986b35884c79a2e52", :filepath=>"/usr/share/libquvi-scripts/0.9/media/guardian.lua", :type=>"media", :domains=>["theguardian.com", "guardian.co.uk"]}
    # {:sha1=>"a82c60a96067232b3b8cdcd864e16e9509fe947d", :filepath=>"/usr/share/libquvi-scripts/0.9/media/tagtele.lua", :type=>"media", :domains=>["tagtele.com"]}
    # {:sha1=>"1d353ae17cf6d7df597fefbfd81cf8c85e282b7d", :filepath=>"/usr/share/libquvi-scripts/0.9/media/publicsenat.lua", :type=>"media", :domains=>["publicsenat.fr"]}
    # {:sha1=>"46000b58b080b4cb8500634087d19ea54dadab63", :filepath=>"/usr/share/libquvi-scripts/0.9/media/lego.lua", :type=>"media", :domains=>["lego.com"]}
    # {:sha1=>"69ee55e3e10c73700c06695055363a6f8a064e5d", :filepath=>"/usr/share/libquvi-scripts/0.9/media/senat.lua", :type=>"media", :domains=>["videos.senat.fr"]}
    # {:sha1=>"c286f739d77da6e1245ab415be7016cf5fad7e0b", :filepath=>"/usr/share/libquvi-scripts/0.9/media/myspass.lua", :type=>"media", :domains=>["myspass.de"]}
    # {:sha1=>"ea16f366f98582d0d52af56cbd3dbe847930dfc3", :filepath=>"/usr/share/libquvi-scripts/0.9/subtitle/youtube.lua", :type=>"subtitle", :domains=>["youtube.com"]}
    # {:sha1=>"7411739ec618bbf3486d98cc0fd64962cfee1239", :filepath=>"/usr/share/libquvi-scripts/0.9/subtitle/export/subrip.lua", :type=>"subtitle_export", :export_format=>"srt"}

To check if particular URL supported by the library, you can use
`Quvi::Handle#supports?` like this:

    require 'quvi'
    handle = Quvi::Handle.new
    handle.supports?("https://soundcloud.com/meta-ex/burning-the-moroder-lamp")
    # => false
    handle.supports?("https://soundcloud.com/meta-ex/burning-the-moroder-lamp", online: true)
    # => true
    handle.supports?("https://www.youtube.com/watch?v=S6-L4xws9l0", type: :playlist)
    # => false
    handle.supports?("https://www.youtube.com/watch?v=S6-L4xws9l0", type: :media)
    # => true
    handle.supports?("https://www.youtube.com/watch?v=S6-L4xws9l0", type: :any)
    # => true

There are also couple of handy methods defined on `Quvi` module, to
simplify argument parsing. First `Quvi.parse_multiple` accepts array
of URLs, dynamically detects type and parse each of them using simple
thread pool. As far as most of the Quvi work is IO, ruby VM can
perfectly optimize it.


## Issues

Please use [github issues](https://github.com/avsej/quvi.rb/issues) to
report any bugs or ideas. Also do not forget to include the version of
the library into report:

    $ ruby -rpp -rrubygems -rquvi -e'pp Quvi::VERSION.to_h'
    {:version=>"0.1.0",
     :quvi_version=>"v0.9.2",
     :quvi_configuration=> "--build=x86_64-redhat-linux-gnu --host=x86_64-redhat-linux-gnu --program-prefix= --disable-dependency-tracking --prefix=/usr --exec-prefix=/usr --bindir=/usr/bin --sbindir=/usr/sbin --sysconfdir=/etc --datadir=/usr/share --includedir=/usr/include --libdir=/usr/lib64 --libexecdir=/usr/libexec --localstatedir=/var --sharedstatedir=/var/lib --mandir=/usr/share/man --infodir=/usr/share/info --enable-static=no --disable-rpath",
     :quvi_build_cc_cflags=> "gcc, -O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches  -m64 -mtune=generic",
     :quvi_build_target=>"x86_64-redhat-linux-gnu",
     :quvi_build_time=>"2013-08-27 04:11:40 +0000"}


## Contributing

1. Fork it ([http://github.com/avsej/quvi.rb/fork](http://github.com/avsej/quvi.rb/fork))
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
