local Uart = {}
Uart.__index = Uart

Uart.DEFAULT_EVENT_ID = 3955961
Uart.DEFAULT_BAUD = 9600

local function check_integer(value, name)
    assert(math.type(value) == "integer", name .. " must be an integer")
    return value
end

function Uart.new(pin, options)
    assert(type(pin) == "table" and type(pin.set) == "function", "UART requires a pin object")
    options = options or {}

    local baud = options.baud or Uart.DEFAULT_BAUD
    assert(type(baud) == "number" and baud > 0, "UART baud must be positive")

    local self = setmetatable({}, Uart)
    self.tx = pin
    self.event_id = check_integer(options.event_id or Uart.DEFAULT_EVENT_ID, "UART event ID")
    self.bit_time = check_integer(options.bit_time or math.floor(SEC / baud + 0.5), "UART bit time")
    assert(self.bit_time > 0, "UART bit time must be positive")
    self.queue = {}
    self.head = 1
    self.tail = 0
    self.data = nil
    self.bit = 0

    -- A UART line must be idle-high before the first start bit. Leaving the
    -- newly created pin floating/low is what corrupted the legacy first frame.
    self.tx:set(1)
    return self
end

setmetatable(Uart, {
    __call = function(_, ...)
        return Uart.new(...)
    end
})

function Uart:send_byte(value)
    value = check_integer(value, "UART byte")
    assert(value >= 0 and value <= 0xff, "UART byte must be from 0 to 255")
    self.tail = self.tail + 1
    self.queue[self.tail] = value
end

function Uart:send_string(value)
    assert(type(value) == "string", "UART payload must be a string")
    for index = 1, #value do
        self:send_byte(string.byte(value, index))
    end
    return #value
end

function Uart:busy()
    return self.data ~= nil or self.head <= self.tail
end

function Uart:send(time)
    time = check_integer(time, "UART time")

    if self.data == nil then
        if self.head > self.tail then
            return false
        end
        self.data = self.queue[self.head]
        self.queue[self.head] = nil
        self.head = self.head + 1
        self.bit = 0
    end

    if self.bit == 0 then
        self.tx:set(0)
    elseif self.bit <= 8 then
        self.tx:set((self.data >> (self.bit - 1)) & 1)
    else
        self.tx:set(1)
    end

    if self.bit == 9 then
        self.data = nil
        self.bit = 0
        if self.head > self.tail then
            return true
        end
    else
        self.bit = self.bit + 1
    end

    set_callback(time + self.bit_time, self.event_id)
    return true
end

return Uart
