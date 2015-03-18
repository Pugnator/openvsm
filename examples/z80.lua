#!/usr/local/bin/lua
--Lua 5.3+ only!


r = {"B","C","D","E","H","L","(HL)","A"}
PC=1
program={0x16, 0xa}


Reg = {}
Reg.__index = Reg

function Reg.new(val)
	local _reg = {}
	setmetatable (_reg, Reg)
	_reg.val = val or 0
	_reg.size = 8
	_reg.B0 = function() return _reg.val >> 0 & 1 end
	_reg.B1 = function() return _reg.val >> 1 & 1 end
	_reg.B2 = function() return _reg.val >> 2 & 1 end
	_reg.B3 = function() return _reg.val >> 3 & 1 end
	_reg.B4 = function() return _reg.val >> 4 & 1 end
	_reg.B5 = function() return _reg.val >> 5 & 1 end
	_reg.B6 = function() return _reg.val >> 6 & 1 end	
	_reg.B7 = function() return _reg.val >> 7 & 1 end
	return _reg
end

--[[
x = the opcode's 1st octal digit (i.e. bits 7-6)
y = the opcode's 2nd octal digit (i.e. bits 5-3)
z = the opcode's 3rd octal digit (i.e. bits 2-0)
p = y rightshifted one position (i.e. bits 5-4)
q = y modulo 2 (i.e. bit 3) 
]]--

Opc = {}
Opc.__index = Opc

function Opc.new(val)
	local opc = {}
	setmetatable(opc, Opc)
	opc.val = val or 0
	opc.x = function() return opc:range(6, 7) end
	opc.y = function() return opc:range(3, 5) end
	opc.z = function() return opc:range(0, 2) end
	opc.p = function() return opc:range(4, 5) end
	opc.q = function() return opc.val >> 3 & 1 end
	return opc
end

function Opc:range(lsb, msb)
	lsb = lsb or 0
	msb = msb or 0
	return (self.val >> lsb) & ~(~0 << (msb-lsb+1))
end


----------------------------------
function getNextByte()
	local result = program[PC]
	PC = PC + 1
	return result
end

function x0 (opcode)
	if 6 == opcode:z() then
		print("LD "..r[opcode:y()]..", "..getNextByte()) --LD r[y], n
	end
end

byte = Opc.new(getNextByte())
print (x0(byte))
