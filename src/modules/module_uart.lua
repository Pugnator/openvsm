-- UART implementation
-- Lua 5.3 compatible

UART_CLOCK_ID = 3955961

Uart = {}
Uart.__index = Uart

setmetatable(Uart, {
  __call = function (cls, ...)
    return cls.new(...)
  end,
})

function Uart.new(pin)
	local self = setmetatable({}, Uart)
	self.bits = 0
	self.data = 0
	self.go = 0
	self.tx = pin
	self.buf = fifo.new()
	return self
end

function Uart:send_string(str)
	for c in str:gmatch"." do
		self.buf:push(string.byte(c))
	end
end

function Uart:send(time)
	if 0 == self.go then
		self.data = self.buf:pop()		
		if nil == self.data then return end
		self.go = 1
	end

	if 0 == self.bits then
		self.tx:set(0)
	elseif 9 == self.bits then
		self.tx:set(1)
	else
		if 1 == bit32.extract(self.data, self.bits - 1) then
			self.tx:set(1)
		else
			self.tx:set(0)
		end
	end

	if 9 == self.bits then
		self.bits = 0
		self.go = 0
		set_callback(time + (150 * USEC), UART_CLOCK_ID)
	else
		set_callback(time + (104 * USEC), UART_CLOCK_ID)
		self.bits = self.bits + 1
	end
end