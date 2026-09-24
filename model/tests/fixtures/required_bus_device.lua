-- A device with a required bus. Setup must stop before device_init when
-- Proteus does not provide the bus.
device_pins = {}
device_buses = {
    {name = "D", base = 0, width = 8}
}

function device_init()
    init_ran = true
end
