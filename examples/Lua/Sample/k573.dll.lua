-- Device description
device_pins =
{
    {is_digital=true, name = "A0", on_time=1000, off_time=1000},
    {is_digital=true, name = "A1", on_time=1000, off_time=1000},
    {is_digital=true, name = "A2", on_time=1000, off_time=1000},
    {is_digital=true, name = "A3", on_time=1000, off_time=1000},
    {is_digital=true, name = "A4", on_time=1000, off_time=1000},
    {is_digital=true, name = "A5", on_time=1000, off_time=1000},
    {is_digital=true, name = "A6", on_time=1000, off_time=1000},
    {is_digital=true, name = "A7", on_time=1000, off_time=1000},
    {is_digital=true, name = "A8", on_time=1000, off_time=1000},
    {is_digital=true, name = "A9", on_time=1000, off_time=1000},
    {is_digital=true, name = "A10", on_time=1000, off_time=1000},
    {is_digital=true, name = "A11", on_time=1000, off_time=1000},
    {is_digital=true, name = "A12", on_time=1000, off_time=1000},
    {is_digital=true, name = "A13", on_time=1000, off_time=1000},
    {is_digital=true, name = "A14", on_time=1000, off_time=1000},
    {is_digital=true, name = "D0", on_time=1000, off_time=1000},
    {is_digital=true, name = "D1", on_time=1000, off_time=1000},
    {is_digital=true, name = "D2", on_time=1000, off_time=1000},
    {is_digital=true, name = "D3", on_time=1000, off_time=1000},
    {is_digital=true, name = "D4", on_time=1000, off_time=1000},
    {is_digital=true, name = "D5", on_time=1000, off_time=1000},
    {is_digital=true, name = "D6", on_time=1000, off_time=1000},
    {is_digital=true, name = "D7", on_time=1000, off_time=1000},
    {is_digital=true, name = "$CE$", on_time=1000, off_time=1000},
    {is_digital=true, name = "$OE$", on_time=1000, off_time=1000},
    {is_digital=true, name = "VPP", on_time=1000, off_time=1000},
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
    rom = read_file(romfile)
end

function device_simulate()
    for i = 0, 14 do
        if 1 == get_pin_bool(_G["A"..i]) then
            ADDRESS = set_bit(ADDRESS, i)
        else
            ADDRESS = clear_bit(ADDRESS, i)
        end
    end
    for i = 0, 7 do
        set_pin_bool(_G["D"..i], get_bit(string.byte(rom, ADDRESS), i))
    end
end

function timer_callback(time, eventid)
end

function on_suspend()
    if nil == mempop then
        mempop, memid = create_memory_popup("My ROM dump")
        set_memory_popup(mempop, rom, string.len(rom))
    elseif mempop then
        repaint_memory_popup(mempop)
    end

    if nil == debugpop then
        debugpop, debugid = create_debug_popup("My ROM vars")
        print_to_debug_popup(debugpop, string.format("Address: %.4X\nData: %.4X\n", ADDRESS, string.byte(rom, ADDRESS)))
        dump_to_debug_popup(debugpop, rom, 32, 0x1000)
    elseif debugpop then
        print_to_debug_popup(debugpop, string.format("Address: %.4X\nData: %.4X\n", ADDRESS, string.byte(rom, ADDRESS)))
        dump_to_debug_popup(debugpop, rom, 32, 0x1000)
    end
end
