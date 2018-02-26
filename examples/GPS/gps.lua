-- GPS hardware part

print = info
SAFE_MODE = true
LOGIC_TYPE = TTL

device_pins = 
{
    {name = "TX", on_time=100000, off_time=100000} 
}

------------------------------------------------


longitude = {}

GPS = {}
GPS.__index = GPS
GPS.lon = {["deg"] = 0,["min"] = 0,["sec"] = 0,["lon"] = 0,["dir"] = "E"}
GPS.lat = {["deg"] = 0,["min"] = 0,["sec"] = 0,["lat"] = 0,["dir"] = "S"}
GPS.speed = 0
GPS.az = 0
GPS.valid = "V"
GPS.pts = 0


--[[ RMC $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
1    = UTC of position fix
2    = Data status (V=navigation receiver warning)
3    = Latitude of fix
4    = N or S
5    = Longitude of fix
6    = E or W
7    = Speed over ground in knots
8    = Track made good in degrees True
9    = UT date
10   = Magnetic variation degrees (Easterly var. subtracts from true course)
11   = E or W
12   = Checksum]]--

RMC = {}
RMC.__index = RMC
RMC.type = "GPRMC"

function RMC:sum(str)
	local sum = 0
	for c in str:gmatch"." do
		sum = sum ~ string.byte(c)
	end
	return sum
end

function RMC:get()
	local str = "%s,%6.3f,%s,%s,%s,%s,%s,%1.1f,%1.1f,%u,%1.1f,%s"
	str = str:format(RMC.type, get_time(), GPS.valid, GPS.lat["lat"], GPS.lat["dir"], GPS.lon["lon"], GPS.lon["dir"], 0, 0, get_date(), 0, GPS.lon["dir"])
	gps = '$' .. str .. '*' .. string.format("%X", RMC:sum(str))
	return gps
end

----------- RMC end

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
	t = os.date("*t")
	return string.format("%.2u%.2u%.2u.000", t.hour, t.min, t.sec)
end

-- get date in GPS string format
function get_date()
		d = os.date("*t")
		return string.format("%.2u%.2u%s", d.day, d.month, string.sub(d.year, -2))
	end

function increment_lat()
	GPS.lat["sec"] = GPS.lat["sec"] + 1
	if GPS.lat["sec"] > 999 then
		GPS.lat["sec"] = 0
		GPS.lat["min"] = GPS.lat["min"] + 1
		if GPS.lat["min"] > 59 then
			GPS.lat["deg"] = GPS.lat["deg"] + 1
		end
	end

	if GPS.lat["deg"] > 90 then
		GPS.lat["deg"] = 0
		--change direction
		if GPS.lat["dir"] == "N" then
			GPS.lat["dir"] = "S"
		else
			GPS.lat["dir"] = "N"
		end
	end
	GPS.lat["lat"] = string.format("%.2u%.2u.%.3u", GPS.lat["deg"], GPS.lat["min"], GPS.lat["sec"])
end

function increment_lon()
	GPS.lon["sec"] = GPS.lon["sec"] + 1
	if GPS.lon["sec"] > 999 then
		GPS.lon["sec"] = 0
		GPS.lon["min"] = GPS.lon["min"] + 1
		if GPS.lon["min"] > 59 then
			GPS.lon["deg"] = GPS.lon["deg"] + 1
		end
	end

	if GPS.lon["deg"] > 180 then
		GPS.lon["deg"] = 0
		--change direction
		if GPS.lon["dir"] == "E" then
			GPS.lon["dir"] = "W"
		else
			GPS.lon["dir"] = "E"
		end
	end
	GPS.lon["lon"] = string.format("%.2u%.2u.%.3u", GPS.lon["deg"], GPS.lon["min"], GPS.lon["sec"])
end

GPS_PPS = 777

function device_init()
	gps_tx = Uart.new(TX)
	TX:set(1)
	set_callback(0, GPS_PPS)
end

function timer_callback( time, eventid)    
    if eventid == GPS_PPS then
		increment_lat()
		increment_lon()
		GPS.pts = GPS.pts + 1
		gps_tx:send_string(RMC.get().."\r\n")
		set_callback(0, UART_CLOCK_ID)
        set_callback(time + SEC, GPS_PPS)
	elseif eventid == UART_CLOCK_ID then
		gps_tx:send(time)
    end
end