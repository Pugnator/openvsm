-- GPS emulator logic

RMC = {}
RMC.__index = RMC

function RMC:lat()
	return 666.66
end

function RMC:lon()
	return 666.66
end

function RMC:sum(str)
	str = str.gsub(',','')
	local sum = 0
	for c in str:gmatch"." do
		sum = sum ~ string.byte(c)
	end
	return sum
end

function RMC:get()
	--$GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
	local str = 'GPRMC,%u,A,%g,S,%g,E,%g,%g,%u,%g,E'
	str = string.format(str, get_time(), RMC:lat(), RMC:lon(), 0, 0, 0, 0, 0)
	print ("Checksum is ", RMC:sum("GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E"))
	return str
end

function os.sleep(msec)
  local now = os.clock() + msec / 1000
  repeat until os.clock() >= now
end

function wait(seconds)
  local now = os.time() + seconds
  repeat until os.time() >= now
end

-- get time in GPS string format
function get_time ()
	d = os.date("*t")
	return string.format("%.2u%.2u%.2u", d.hour, d.min, d.sec)
end

while(true)
do
	wait(1)
	print(RMC.get())
end