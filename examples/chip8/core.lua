local Chip8 = {}
Chip8.__index = Chip8

Chip8.MEMORY_SIZE = 4096
Chip8.PROGRAM_ADDRESS = 0x200
Chip8.FONT_ADDRESS = 0x050
Chip8.DISPLAY_WIDTH = 64
Chip8.DISPLAY_HEIGHT = 32
Chip8.STACK_DEPTH = 16

local font = {
    0xf0, 0x90, 0x90, 0x90, 0xf0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xf0, 0x10, 0xf0, 0x80, 0xf0,
    0xf0, 0x10, 0xf0, 0x10, 0xf0,
    0x90, 0x90, 0xf0, 0x10, 0x10,
    0xf0, 0x80, 0xf0, 0x10, 0xf0,
    0xf0, 0x80, 0xf0, 0x90, 0xf0,
    0xf0, 0x10, 0x20, 0x40, 0x40,
    0xf0, 0x90, 0xf0, 0x90, 0xf0,
    0xf0, 0x90, 0xf0, 0x10, 0xf0,
    0xf0, 0x90, 0xf0, 0x90, 0x90,
    0xe0, 0x90, 0xe0, 0x90, 0xe0,
    0xf0, 0x80, 0x80, 0x80, 0xf0,
    0xe0, 0x90, 0x90, 0x90, 0xe0,
    0xf0, 0x80, 0xf0, 0x80, 0xf0,
    0xf0, 0x80, 0xf0, 0x80, 0x80
}

local known_quirks = {
    clip_sprites = true,
    jump_uses_vx = true,
    load_store_increment_i = true,
    shift_uses_vy = true
}

local function check_integer(name, value, minimum, maximum)
    if math.type(value) ~= "integer" or value < minimum or value > maximum then
        error(string.format("%s must be an integer from %d to %d", name, minimum, maximum), 3)
    end
    return value
end

local function copy_quirks(options)
    local profile = options.profile or "modern"
    if profile ~= "modern" and profile ~= "original" then
        error("CHIP-8 profile must be 'modern' or 'original'", 3)
    end

    local quirks = {
        clip_sprites = false,
        jump_uses_vx = false,
        load_store_increment_i = profile == "original",
        shift_uses_vy = profile == "original"
    }

    for name, enabled in pairs(options.quirks or {}) do
        if not known_quirks[name] then
            error("unknown CHIP-8 quirk: " .. tostring(name), 3)
        end
        quirks[name] = not not enabled
    end
    return quirks
end

local function check_display(display)
    if display == nil or type(display.clear) ~= "function" or type(display.xor_pixel) ~= "function" then
        error("CHIP-8 display must provide clear() and xor_pixel(x, y)", 3)
    end
end

local function invalid_opcode(vm, opcode)
    error(string.format("unsupported CHIP-8 opcode 0x%04X at 0x%03X", opcode, vm.current_address), 0)
end

local function read_memory(vm, address)
    if math.type(address) ~= "integer" or address < 0 or address >= Chip8.MEMORY_SIZE then
        error(string.format("CHIP-8 memory read outside 0x000..0xFFF: 0x%X", address), 0)
    end
    return vm.memory[address]
end

local function write_memory(vm, address, value)
    if math.type(address) ~= "integer" or address < 0 or address >= Chip8.MEMORY_SIZE then
        error(string.format("CHIP-8 memory write outside 0x000..0xFFF: 0x%X", address), 0)
    end
    vm.memory[address] = value & 0xff
end

local function first_pressed_key(vm)
    for key = 0, 15 do
        if vm.keys[key] then
            return key
        end
    end
    return nil
end

local function draw_sprite(vm, x_register, y_register, rows)
    local origin_x = vm.v[x_register]
    local origin_y = vm.v[y_register]
    local collision = false

    for row = 0, rows - 1 do
        local sprite = read_memory(vm, vm.i + row)
        for bit = 0, 7 do
            if (sprite & (0x80 >> bit)) ~= 0 then
                local x = origin_x + bit
                local y = origin_y + row
                if vm.quirks.clip_sprites then
                    if x < Chip8.DISPLAY_WIDTH and y < Chip8.DISPLAY_HEIGHT then
                        collision = vm.display:xor_pixel(x, y) or collision
                    end
                else
                    x = x % Chip8.DISPLAY_WIDTH
                    y = y % Chip8.DISPLAY_HEIGHT
                    collision = vm.display:xor_pixel(x, y) or collision
                end
            end
        end
    end

    vm.v[0xf] = collision and 1 or 0
end

function Chip8.new(options)
    options = options or {}
    check_display(options.display)

    local vm = setmetatable({
        display = options.display,
        quirks = copy_quirks(options),
        random_byte = options.random_byte or function()
            return math.random(0, 255)
        end,
        memory = {},
        v = {},
        stack = {},
        keys = {}
    }, Chip8)

    if type(vm.random_byte) ~= "function" then
        error("CHIP-8 random_byte must be a function", 2)
    end

    vm:reset()
    return vm
end

function Chip8:reset()
    for address = 0, Chip8.MEMORY_SIZE - 1 do
        self.memory[address] = 0
    end
    for register = 0, 15 do
        self.v[register] = 0
        self.keys[register] = false
    end
    for level = 1, Chip8.STACK_DEPTH do
        self.stack[level] = 0
    end
    for offset, value in ipairs(font) do
        self.memory[Chip8.FONT_ADDRESS + offset - 1] = value
    end

    self.i = 0
    self.pc = Chip8.PROGRAM_ADDRESS
    self.sp = 0
    self.delay_timer = 0
    self.sound_timer = 0
    self.cycles = 0
    self.last_opcode = nil
    self.current_address = self.pc
    self.display:clear()
end

function Chip8:load_rom(rom, address)
    address = address or Chip8.PROGRAM_ADDRESS
    check_integer("ROM address", address, 0, Chip8.MEMORY_SIZE - 1)

    local bytes = {}
    if type(rom) == "string" then
        for index = 1, #rom do
            bytes[index] = string.byte(rom, index)
        end
    elseif type(rom) == "table" then
        for index = 1, #rom do
            bytes[index] = check_integer("ROM byte", rom[index], 0, 255)
        end
    else
        error("CHIP-8 ROM must be a byte string or array", 2)
    end

    if #bytes > Chip8.MEMORY_SIZE - address then
        error("CHIP-8 ROM does not fit in memory", 2)
    end

    self:reset()
    for offset, value in ipairs(bytes) do
        self.memory[address + offset - 1] = value
    end
    self.pc = address
    return #bytes
end

function Chip8:peek(address)
    check_integer("memory address", address, 0, Chip8.MEMORY_SIZE - 1)
    return self.memory[address]
end

function Chip8:poke(address, value)
    check_integer("memory address", address, 0, Chip8.MEMORY_SIZE - 1)
    check_integer("memory value", value, 0, 255)
    self.memory[address] = value
end

function Chip8:set_key(key, pressed)
    check_integer("key", key, 0, 15)
    self.keys[key] = not not pressed
end

function Chip8:tick_timers(ticks)
    ticks = ticks or 1
    check_integer("timer ticks", ticks, 0, 0x7fffffff)

    local was_sounding = self.sound_timer > 0
    self.delay_timer = math.max(0, self.delay_timer - ticks)
    self.sound_timer = math.max(0, self.sound_timer - ticks)
    return was_sounding, self.sound_timer > 0
end

function Chip8:snapshot()
    local registers = {}
    for register = 0, 15 do
        registers[register] = self.v[register]
    end
    return {
        cycles = self.cycles,
        delay_timer = self.delay_timer,
        i = self.i,
        pc = self.pc,
        registers = registers,
        sound_timer = self.sound_timer,
        sp = self.sp
    }
end

function Chip8:run(cycles)
    check_integer("cycles", cycles, 0, 0x7fffffff)
    for _ = 1, cycles do
        self:step()
    end
end

function Chip8:step()
    if self.pc < 0 or self.pc + 1 >= Chip8.MEMORY_SIZE then
        error(string.format("CHIP-8 instruction fetch outside memory at 0x%X", self.pc), 0)
    end

    local instruction_address = self.pc
    local opcode = (read_memory(self, self.pc) << 8) | read_memory(self, self.pc + 1)
    self.pc = self.pc + 2
    self.current_address = instruction_address
    self.last_opcode = opcode

    local group = opcode & 0xf000
    local x = (opcode >> 8) & 0xf
    local y = (opcode >> 4) & 0xf
    local n = opcode & 0xf
    local byte = opcode & 0xff
    local address = opcode & 0xfff

    if opcode == 0x00e0 then
        self.display:clear()
    elseif opcode == 0x00ee then
        if self.sp == 0 then
            error("CHIP-8 stack underflow", 0)
        end
        self.pc = self.stack[self.sp]
        self.stack[self.sp] = 0
        self.sp = self.sp - 1
    elseif group == 0x0000 then
        -- 0NNN called an RCA 1802 routine. Modern hosts safely ignore it.
    elseif group == 0x1000 then
        self.pc = address
    elseif group == 0x2000 then
        if self.sp >= Chip8.STACK_DEPTH then
            error("CHIP-8 stack overflow", 0)
        end
        self.sp = self.sp + 1
        self.stack[self.sp] = self.pc
        self.pc = address
    elseif group == 0x3000 then
        if self.v[x] == byte then
            self.pc = self.pc + 2
        end
    elseif group == 0x4000 then
        if self.v[x] ~= byte then
            self.pc = self.pc + 2
        end
    elseif group == 0x5000 and n == 0 then
        if self.v[x] == self.v[y] then
            self.pc = self.pc + 2
        end
    elseif group == 0x6000 then
        self.v[x] = byte
    elseif group == 0x7000 then
        self.v[x] = (self.v[x] + byte) & 0xff
    elseif group == 0x8000 then
        local vx = self.v[x]
        local vy = self.v[y]
        if n == 0x0 then
            self.v[x] = vy
        elseif n == 0x1 then
            self.v[x] = vx | vy
        elseif n == 0x2 then
            self.v[x] = vx & vy
        elseif n == 0x3 then
            self.v[x] = vx ~ vy
        elseif n == 0x4 then
            local sum = vx + vy
            self.v[x] = sum & 0xff
            self.v[0xf] = sum > 0xff and 1 or 0
        elseif n == 0x5 then
            self.v[x] = (vx - vy) & 0xff
            self.v[0xf] = vx >= vy and 1 or 0
        elseif n == 0x6 then
            local source = self.quirks.shift_uses_vy and vy or vx
            self.v[x] = source >> 1
            self.v[0xf] = source & 0x1
        elseif n == 0x7 then
            self.v[x] = (vy - vx) & 0xff
            self.v[0xf] = vy >= vx and 1 or 0
        elseif n == 0xe then
            local source = self.quirks.shift_uses_vy and vy or vx
            self.v[x] = (source << 1) & 0xff
            self.v[0xf] = (source >> 7) & 0x1
        else
            invalid_opcode(self, opcode)
        end
    elseif group == 0x9000 and n == 0 then
        if self.v[x] ~= self.v[y] then
            self.pc = self.pc + 2
        end
    elseif group == 0xa000 then
        self.i = address
    elseif group == 0xb000 then
        local base_register = self.quirks.jump_uses_vx and x or 0
        self.pc = (address + self.v[base_register]) & 0xfff
    elseif group == 0xc000 then
        local random_value = self.random_byte()
        check_integer("random byte", random_value, 0, 255)
        self.v[x] = random_value & byte
    elseif group == 0xd000 then
        draw_sprite(self, x, y, n)
    elseif group == 0xe000 then
        local pressed = self.keys[self.v[x]] == true
        if byte == 0x9e then
            if pressed then
                self.pc = self.pc + 2
            end
        elseif byte == 0xa1 then
            if not pressed then
                self.pc = self.pc + 2
            end
        else
            invalid_opcode(self, opcode)
        end
    elseif group == 0xf000 then
        if byte == 0x07 then
            self.v[x] = self.delay_timer
        elseif byte == 0x0a then
            local key = first_pressed_key(self)
            if key == nil then
                self.pc = instruction_address
            else
                self.v[x] = key
            end
        elseif byte == 0x15 then
            self.delay_timer = self.v[x]
        elseif byte == 0x18 then
            self.sound_timer = self.v[x]
        elseif byte == 0x1e then
            self.i = (self.i + self.v[x]) & 0xfff
        elseif byte == 0x29 then
            self.i = Chip8.FONT_ADDRESS + (self.v[x] & 0xf) * 5
        elseif byte == 0x33 then
            local value = self.v[x]
            write_memory(self, self.i, value // 100)
            write_memory(self, self.i + 1, (value // 10) % 10)
            write_memory(self, self.i + 2, value % 10)
        elseif byte == 0x55 then
            for register = 0, x do
                write_memory(self, self.i + register, self.v[register])
            end
            if self.quirks.load_store_increment_i then
                self.i = (self.i + x + 1) & 0xfff
            end
        elseif byte == 0x65 then
            for register = 0, x do
                self.v[register] = read_memory(self, self.i + register)
            end
            if self.quirks.load_store_increment_i then
                self.i = (self.i + x + 1) & 0xfff
            end
        else
            invalid_opcode(self, opcode)
        end
    else
        invalid_opcode(self, opcode)
    end

    self.cycles = self.cycles + 1
    return opcode
end

return Chip8
