-- Stack Table
-- Uses a table as stack, use <table>:push(value) and <table>:pop()
-- Lua 5.3 compatible

-- GLOBAL
fifo = {}
fifo.__index = fifo

setmetatable(fifo, {
  __call = function (cls, ...)
    return cls.new(...)
  end,
})

-- Create a Table with stack functions
function fifo.new()
	local self = setmetatable({}, fifo)
	self.stack = {}
	return self
end


-- push a value on to the stack
function fifo:push(...)
if ... then
  local targs = {...}
  -- add values at the END
  for _,v in ipairs(targs) do
	table.insert(self.stack, 1, v)
  end
end
end

-- pop a value from the stack
function fifo:pop(num)

-- get num values from stack
local num = num or 1

-- return table
local entries = {}

-- get values into entries
for i = 1, num do
  -- get last entry
  if #self.stack ~= 0 then
	table.insert(entries, self.stack[#self.stack])
	-- remove last value
	table.remove(self.stack)
  else
	break
  end
end
-- return unpacked entries
return table.unpack(entries)
end

-- get entries
function fifo:getn()
return #self.stack
end

-- list values
function fifo:list()
for i,v in pairs(self.stack) do
  info(i, v)
end
end
