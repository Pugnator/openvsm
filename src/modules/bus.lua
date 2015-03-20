--IC Bus object

Bus = {}
Bus.__index = Bus

function Bus.new (pins)
	local _bus = {}
	setmetatable(_bus,Bus)
	_bus.pins = pins
	return _bus
end

function Bus:get ()
	return get_bus(self.pins)
end

function Bus:set (val)
	set_bus(self.pins, val)
end