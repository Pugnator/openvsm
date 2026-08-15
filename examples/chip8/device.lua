device_pins = {}

local BODY_WIDTH = 344
local BODY_HEIGHT = 166
local SCREEN_LEFT = 12
local SCREEN_TOP = 30
local SCREEN_RIGHT = 204
local SCREEN_BOTTOM = 126
local SCREEN_SCALE = 3
local KEYPAD_LEFT = 220
local KEYPAD_TOP = 30
local KEY_SIZE = 24
local KEY_GAP = 4

local TIMER_HZ = 60
local CPU_HZ = 700
local TIMER_EVENT = 0xc817

local function sibling_path(name)
    local source = debug.getinfo(1, "S").source
    if source:sub(1, 1) == "@" then
        source = source:sub(2)
    end
    local directory = source:match("^(.*[\\/])")
    assert(directory ~= nil, "CHIP-8 device script directory is unavailable")
    return directory .. name
end

local Chip8 = assert(dofile(sibling_path("core.lua")))
local Framebuffer = assert(dofile(sibling_path("framebuffer.lua")))

local keys = {
    {label = "1", value = 0x1},
    {label = "2", value = 0x2},
    {label = "3", value = 0x3},
    {label = "C", value = 0xc},
    {label = "4", value = 0x4},
    {label = "5", value = 0x5},
    {label = "6", value = 0x6},
    {label = "D", value = 0xd},
    {label = "7", value = 0x7},
    {label = "8", value = 0x8},
    {label = "9", value = 0x9},
    {label = "E", value = 0xe},
    {label = "A", value = 0xa},
    {label = "0", value = 0x0},
    {label = "B", value = 0xb},
    {label = "F", value = 0xf}
}

local keyboard_layout = {
    {key = "1", value = 0x1}, {key = "2", value = 0x2}, {key = "3", value = 0x3}, {key = "4", value = 0xc},
    {key = "Q", value = 0x4}, {key = "W", value = 0x5}, {key = "E", value = 0x6}, {key = "R", value = 0xd},
    {key = "A", value = 0x7}, {key = "S", value = 0x8}, {key = "D", value = 0x9}, {key = "F", value = 0xe},
    {key = "Z", value = 0xa}, {key = "X", value = 0x0}, {key = "C", value = 0xb}, {key = "V", value = 0xf}
}
local keyboard = {}
for _, binding in ipairs(keyboard_layout) do
    keyboard[string.byte(binding.key)] = binding.value
    keyboard[string.byte(binding.key:lower())] = binding.value
end

-- This original 38-byte program draws the sixteen hexadecimal font glyphs.
-- It makes a newly placed device visibly useful without distributing a ROM.
local DEMO_ROM = string.char(
    0x00, 0xe0, 0x60, 0x02, 0x61, 0x02, 0x62, 0x00,
    0xf2, 0x29, 0xd0, 0x15, 0x70, 0x08, 0x72, 0x01,
    0x32, 0x08, 0x12, 0x08, 0x60, 0x02, 0x61, 0x0c,
    0xf2, 0x29, 0xd0, 0x15, 0x70, 0x08, 0x72, 0x01,
    0x32, 0x10, 0x12, 0x18, 0x12, 0x24)

local framebuffer
local vm
local running = false
local fault_message
local rom_label = "DEMO"
local selected_key
local pending_key_releases = {}
local cpu_remainder = 0
local timer_remainder = 0

local timer_base = math.floor(SEC / TIMER_HZ)
local timer_fraction = SEC % TIMER_HZ

local function next_timer_interval()
    timer_remainder = timer_remainder + timer_fraction
    local correction = 0
    if timer_remainder >= TIMER_HZ then
        timer_remainder = timer_remainder - TIMER_HZ
        correction = 1
    end
    return timer_base + correction
end

local function next_cpu_slice()
    cpu_remainder = cpu_remainder + CPU_HZ
    local instructions = math.floor(cpu_remainder / TIMER_HZ)
    cpu_remainder = cpu_remainder % TIMER_HZ
    return instructions
end

local function repaint()
    if graphics ~= nil then
        graphics.repaint(false)
    end
end

local function stop_with_error(message)
    running = false
    fault_message = tostring(message)
    print("CHIP-8 stopped: " .. fault_message)
    if type(vsm_error) == "function" then
        vsm_error("CHIP-8 stopped: " .. fault_message)
    end
    repaint()
end

local function read_rom(path)
    local file, open_error = io.open(path, "rb")
    if file == nil then
        return nil, open_error
    end

    local bytes, read_error = file:read("*a")
    file:close()
    if bytes == nil then
        return nil, read_error
    end
    return bytes
end

local function configured_rom()
    local path = get_string_param("ROM")
    if type(path) ~= "string" or path:match("%S") == nil then
        return DEMO_ROM, "DEMO"
    end

    local bytes, load_error = read_rom(path)
    if bytes == nil then
        return nil, string.format("cannot read ROM '%s': %s", path, tostring(load_error))
    end
    return bytes, path:match("([^\\/]+)$") or path
end

local function release_keys()
    if vm ~= nil then
        for value in pairs(pending_key_releases) do
            vm:set_key(value, false)
        end
    end
    pending_key_releases = {}
    if selected_key ~= nil then
        selected_key = nil
        repaint()
    end
end

local function press_key(value)
    if not running or vm == nil then
        return false
    end
    vm:set_key(value, true)
    pending_key_releases[value] = true
    selected_key = value
    repaint()
    return true
end

local function key_bounds(index)
    local zero_based = index - 1
    local column = zero_based % 4
    local row = math.floor(zero_based / 4)
    local left = KEYPAD_LEFT + column * (KEY_SIZE + KEY_GAP)
    local top = KEYPAD_TOP + row * (KEY_SIZE + KEY_GAP)
    return left, top, left + KEY_SIZE, top + KEY_SIZE
end

local function draw_key(index, key)
    local left, top, right, bottom = key_bounds(index)
    local selected = selected_key == key.value

    graphics.set_pen_colour(selected and graphics.BRIGHTGREEN or graphics.WHITE)
    graphics.set_brush_colour(selected and graphics.BRIGHTGREEN or graphics.BLACK)
    graphics.draw_box(left, top, right, bottom)
    graphics.set_text_colour(selected and graphics.BLACK or graphics.BRIGHTWHITE)
    graphics.draw_text(math.floor((left + right) / 2), math.floor((top + bottom) / 2), 0,
                       graphics.TXJ_CENTRE | graphics.TXJ_MIDDLE, key.label)
end

local function draw_framebuffer()
    if framebuffer == nil then
        return
    end

    local snapshot = framebuffer:snapshot()
    graphics.set_pen_colour(graphics.BRIGHTGREEN)
    graphics.set_brush_colour(graphics.BRIGHTGREEN)
    for index, pixel in ipairs(snapshot.pixels) do
        if pixel ~= 0 then
            local zero_based = index - 1
            local x = zero_based % snapshot.width
            local y = math.floor(zero_based / snapshot.width)
            local left = SCREEN_LEFT + x * SCREEN_SCALE
            local top = SCREEN_TOP + y * SCREEN_SCALE
            graphics.draw_box(left, top, left + SCREEN_SCALE, top + SCREEN_SCALE)
        end
    end
end

function device_init()
    framebuffer = Framebuffer.new(Chip8.DISPLAY_WIDTH, Chip8.DISPLAY_HEIGHT)
    vm = Chip8.new({
        display = framebuffer,
        profile = "modern",
        random_byte = function()
            return math.random(0, 255)
        end
    })
    running = false
    fault_message = nil
    selected_key = nil
    pending_key_releases = {}
    cpu_remainder = 0
    timer_remainder = 0

    local rom, label_or_error = configured_rom()
    if rom == nil then
        rom_label = "ROM ERROR"
        stop_with_error(label_or_error)
        return
    end

    local loaded, load_error = pcall(vm.load_rom, vm, rom)
    if not loaded then
        rom_label = "ROM ERROR"
        stop_with_error(load_error)
        return
    end

    rom_label = label_or_error
    running = true
    print(string.format("CHIP-8 started: %s (%d bytes, %d instructions/s)", rom_label, #rom, CPU_HZ))
    set_callback(systime() + next_timer_interval(), TIMER_EVENT)
end

function device_simulate()
end

function timer_callback(time, event_id)
    if event_id ~= TIMER_EVENT or not running then
        return
    end

    local executed, execution_error = pcall(function()
        vm:run(next_cpu_slice())
        vm:tick_timers(1)
    end)
    release_keys()

    if not executed then
        stop_with_error(execution_error)
        return
    end

    if framebuffer:consume_dirty() then
        repaint()
    end
    set_callback(time + next_timer_interval(), TIMER_EVENT)
end

function device_graphics_init()
    graphics.set_draw_scale(96)
    graphics.set_pen_width(1)
    graphics.set_text_size(12)
end

function device_graphics_plot(_)
    graphics.set_pen_colour(graphics.BRIGHTWHITE)
    graphics.set_brush_colour(graphics.GREY)
    graphics.draw_box(0, 0, BODY_WIDTH, BODY_HEIGHT)

    graphics.set_text_colour(graphics.BRIGHTWHITE)
    graphics.draw_text(12, 14, 0, graphics.TXJ_LEFT | graphics.TXJ_MIDDLE, "CHIP-8")
    graphics.draw_text(BODY_WIDTH - 12, 14, 0, graphics.TXJ_RIGHT | graphics.TXJ_MIDDLE, rom_label)

    graphics.set_pen_colour(graphics.BRIGHTGREEN)
    graphics.set_brush_colour(graphics.BLACK)
    graphics.draw_box(SCREEN_LEFT, SCREEN_TOP, SCREEN_RIGHT, SCREEN_BOTTOM)
    draw_framebuffer()

    if not running then
        graphics.set_text_colour(graphics.BRIGHTGREEN)
        graphics.set_text_size(14)
        graphics.draw_text(math.floor((SCREEN_LEFT + SCREEN_RIGHT) / 2),
                           math.floor((SCREEN_TOP + SCREEN_BOTTOM) / 2), 0,
                           graphics.TXJ_CENTRE | graphics.TXJ_MIDDLE,
                           fault_message == nil and "STOPPED" or "ROM ERROR")
    end

    graphics.set_text_size(10)
    graphics.set_text_colour(graphics.BRIGHTWHITE)
    local state = running and "RUN" or (fault_message == nil and "STOP" or "ERROR")
    local pc = vm == nil and Chip8.PROGRAM_ADDRESS or vm.pc
    graphics.draw_text(SCREEN_LEFT, 144, 0, graphics.TXJ_LEFT | graphics.TXJ_MIDDLE,
                       string.format("%s  PC:%04X  %d Hz  %d ips", state, pc, TIMER_HZ, CPU_HZ))

    for index, key in ipairs(keys) do
        draw_key(index, key)
    end
end

function device_graphics_animate(_, _, _)
end

function device_graphics_actuate(key, x, y, flags)
    local keyboard_value = keyboard[key]
    if keyboard_value ~= nil then
        return press_key(keyboard_value)
    end

    if (flags & graphics.ACF_LEFT) == 0 then
        return false
    end

    for index, keypad_key in ipairs(keys) do
        local left, top, right, bottom = key_bounds(index)
        if x >= left and x <= right and y >= top and y <= bottom then
            return press_key(keypad_key.value)
        end
    end

    return false
end
