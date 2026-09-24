-- A device with an optional bus. Setup must complete when Proteus does not
-- provide the bus, leaving the bus global nil.
device_pins = {}
device_buses = {
    {name = "D", base = 0, width = 8, required = false}
}

function device_init()
    init_ran = true
    bus_was_nil = (D == nil)
end
