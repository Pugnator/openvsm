-- Device description
SAFE_MODE = true
LOGIC_TYPE = TTL

device_pins =
{
    {name = "A0", on_time=1000, off_time=1000},
    {name = "A1", on_time=1000, off_time=1000},
    {name = "A2", on_time=1000, off_time=1000},
    {name = "A3", on_time=1000, off_time=1000},
    {name = "A4", on_time=1000, off_time=1000},
    {name = "A5", on_time=1000, off_time=1000},
    {name = "A6", on_time=1000, off_time=1000},
    {name = "A7", on_time=1000, off_time=1000},
    {name = "A8", on_time=1000, off_time=1000},
    {name = "A9", on_time=1000, off_time=1000},
    {name = "A10", on_time=1000, off_time=1000},
    {name = "A11", on_time=1000, off_time=1000},
    {name = "A12", on_time=1000, off_time=1000},
    {name = "A13", on_time=1000, off_time=1000},
    {name = "A14", on_time=1000, off_time=1000},
    {name = "D0", on_time=1000, off_time=1000},
    {name = "D1", on_time=1000, off_time=1000},
    {name = "D2", on_time=1000, off_time=1000},
    {name = "D3", on_time=1000, off_time=1000},
    {name = "D4", on_time=1000, off_time=1000},
    {name = "D5", on_time=1000, off_time=1000},
    {name = "D6", on_time=1000, off_time=1000},
    {name = "D7", on_time=1000, off_time=1000},
    {name = "$CE$", on_time=1000, off_time=1000},
    {name = "$OE$", on_time=1000, off_time=1000},
    {name = "VPP", on_time=1000, off_time=1000},
}

ADDRESS = 0

function read_file(file)
    local file = io.open(file, "rb")
    local content = file:read("*a")
    file:close()
    return content
end

function device_init()
    local romfile = get_string_param("file")
	if romfile == "?" then error("No romfile specified") return end
    rom = read_file(romfile)
	ABUS = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14}
	DBUS = {D0, D1, D2, D3, D4, D5, D6, D7}	
	--mempop, memid = create_memory_popup("My ROM dump")
end

function device_simulate()			
	ADDRESS = get_bus (ABUS)	
	if ADDRESS == nil then return end
	local cur_byte = string.byte(rom, ADDRESS+1)		
	if 1 == get_pin(CE) and 1 == get_pin(OE) then return end
	if cur_byte then
	set_bus(DBUS, cur_byte)
	info("ROM data:"..cur_byte)
	end
end
