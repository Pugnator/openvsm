device_pins = {
    {name = "A", on_time = 100000, off_time = 110000},
    {name = "B", on_time = 200000, off_time = 210000},
    {name = "Q", on_time = 300000, off_time = 310000}
}

initialized = false
last_callback_time = nil
last_callback_id = nil

function device_init()
    initialized = true
    Q:set(1)
end

function device_simulate()
    Q:set(1 - (A:get() * B:get()))
end

function timer_callback(time, event_id)
    last_callback_time = time
    last_callback_id = event_id
end
