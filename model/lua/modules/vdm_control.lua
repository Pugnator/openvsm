local Control = {}
Control.__index = Control

local TARGET_INFO_FORMAT = "<!4c32I4I4I4I4dI4I4c64"

local function integer(value, name, minimum, maximum)
    assert(math.type(value) == "integer", name .. " must be an integer")
    assert(value >= minimum and value <= maximum, name .. " is out of range")
    return value
end

local function fixed_string(value, size, name)
    assert(type(value) == "string", name .. " must be a string")
    assert(#value <= size, name .. " is too long")
    return value .. string.rep("\0", size - #value)
end

function Control.new(options)
    options = options or {}

    local self = setmetatable({}, Control)
    self.id = fixed_string(options.id or "OpenVSM", 31, "target ID")
    self.regclass = integer(options.regclass or 0, "register class", 0, 0xffffffff)
    self.regsize = integer(options.regsize or 0, "register size", 0, 0xffffffff)
    self.apiver = integer(options.apiver or VDM_API_VERSION, "API version", 0, 0xffffffff)
    self.dllver = integer(options.dllver or 0, "DLL version", 0, 0xffffffff)
    self.clock = options.clock or 0
    assert(type(self.clock) == "number" and self.clock >= 0, "clock must be a non-negative number")
    self.specific = fixed_string(options.specific or "", 64, "target-specific data")
    self.registers = fixed_string(options.registers or "", self.regsize, "register data")
    self.pc = integer(options.pc or 0, "program counter", 0, 0xffffffff)
    self.spaces = {}
    self.breakpoints = {}
    self.callbacks = {}
    return self
end

function Control:register_space(memspace, size, initial)
    memspace = integer(memspace, "memory space", 0, 0xff)
    size = integer(size, "memory size", 0, 0xffffffff)
    initial = initial or ""
    assert(type(initial) == "string", "initial memory must be a string")
    assert(#initial <= size, "initial memory is larger than the memory space")
    self.spaces[memspace] = initial .. string.rep("\0", size - #initial)
    return self
end

function Control:read(memspace, address, length)
    memspace = integer(memspace, "memory space", 0, 0xff)
    address = integer(address, "address", 0, 0xffffffff)
    length = integer(length, "length", 0, 0xffffffff)
    local memory = self.spaces[memspace]
    if memory == nil or address > #memory or length > #memory - address then
        return nil, ERR_VDM_BADADDRESS
    end
    return memory:sub(address + 1, address + length), ERR_VDM_OK
end

function Control:write(memspace, address, payload)
    memspace = integer(memspace, "memory space", 0, 0xff)
    address = integer(address, "address", 0, 0xffffffff)
    assert(type(payload) == "string", "payload must be a string")
    local memory = self.spaces[memspace]
    if memory == nil or address > #memory or #payload > #memory - address then
        return ERR_VDM_BADADDRESS
    end
    self.spaces[memspace] = memory:sub(1, address) .. payload .. memory:sub(address + #payload + 1)
    return ERR_VDM_OK
end

function Control:on(command, callback)
    command = integer(command, "command", 0, 0xff)
    assert(type(callback) == "function", "command callback must be a function")
    self.callbacks[command] = callback
    return self
end

function Control:target_info()
    return string.pack(TARGET_INFO_FORMAT, self.id, self.regclass, self.regsize, self.apiver, self.dllver, self.clock,
        0, 0, self.specific)
end

function Control:dispatch(command, memspace, address, length, payload)
    local callback = self.callbacks[command]
    if callback ~= nil then
        local result, response = callback(self, memspace, address, length, payload)
        if result ~= nil then
            return result, response
        end
    end

    if command == VDM_INIT then
        return ERR_VDM_OK, self:target_info()
    elseif command == VDM_TERM or command == VDM_PLAY or command == VDM_STEP or command == VDM_PAUSE or
        command == VDM_RESET then
        return ERR_VDM_OK
    elseif command == VDM_GETTID then
        return ERR_VDM_OK, fixed_string(self.id, 64, "target ID")
    elseif command == VDM_READDATA then
        local response, result = self:read(memspace, address, length)
        return result, response
    elseif command == VDM_WRITEDATA then
        if #payload ~= length then
            return ERR_VDM_BADDATALEN
        end
        return self:write(memspace, address, payload)
    elseif command == VDM_READREGS then
        if length > #self.registers then
            return ERR_VDM_BADDATALEN
        end
        return ERR_VDM_OK, self.registers:sub(1, length)
    elseif command == VDM_WRITEREGS then
        if length ~= #self.registers or #payload ~= length then
            return ERR_VDM_BADDATALEN
        end
        self.registers = payload
        return ERR_VDM_OK
    elseif command == VDM_SETPC then
        self.pc = integer(address, "program counter", 0, 0xffffffff)
        return ERR_VDM_OK
    elseif command == VDM_GETPC then
        if length < 4 then
            return ERR_VDM_BADDATALEN
        end
        return ERR_VDM_OK, string.pack("<I4", self.pc)
    elseif command == VDM_SETBP then
        self.breakpoints[address] = true
        return ERR_VDM_OK
    elseif command == VDM_CLRBP then
        self.breakpoints[address] = nil
        return ERR_VDM_OK
    end

    return ERR_VDM_BADCOMMAND
end

function Control:install()
    local target = self
    _G.device_vdm_command = function(command, memspace, address, length, payload)
        return target:dispatch(command, memspace, address, length, payload)
    end
    return self
end

return Control
