-- UART implementation
-- Lua 5.3 compatible
--[[
UART_CLOCK_ID = 3955961

Uart = {}
function Uart:new(pin)

	local t = {}
	t.bitn = 0
	t.data = 0
	t.go = 0
	t.tx = pin
	t.buf = fifo:new()

	function t:send_string(str)
		for c in str:gmatch"." do
			buf:push(string.byte(c))
		end
	end

	function t:send(time)
		if 0 == go then
			data = buf:pop()
			if nil == data then return end
			go = 1
		end

		if 0 == bitn then
			tx:set(0)
		elseif 9 == bitn then
			tx:set(1)
		else
			if 1 == bitn32.extract(data, bitn - 1) then
				tx:set(1)
			else
				tx:set(0)
			end
		end

		if 9 == bitn then
			bitn = 0
			go = 0
			set_callback(time + (150 * USEC), UART_CLOCK_ID)
		else
			set_callback(time + (104 * USEC), UART_CLOCK_ID)
			bitn = bitn + 1
		end
	end
	return t
end
]]--