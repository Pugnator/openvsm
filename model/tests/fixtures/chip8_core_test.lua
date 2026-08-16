local Chip8 = assert(dofile(CHIP8_CORE_PATH))
local Framebuffer = assert(dofile(CHIP8_FRAMEBUFFER_PATH))

local checks = 0

local function check(condition, message)
    checks = checks + 1
    assert(condition, message)
end

local function expect_error(fragment, action)
    local success, message = pcall(action)
    check(not success, "expected an error containing: " .. fragment)
    check(tostring(message):find(fragment, 1, true) ~= nil, tostring(message))
end

local function encode(words)
    local bytes = {}
    for _, word in ipairs(words) do
        bytes[#bytes + 1] = string.char((word >> 8) & 0xff, word & 0xff)
    end
    return table.concat(bytes)
end

local function machine(words, options)
    options = options or {}
    local display = options.display or Framebuffer.new(Chip8.DISPLAY_WIDTH, Chip8.DISPLAY_HEIGHT)
    options.display = display
    local vm = Chip8.new(options)
    vm:load_rom(encode(words or {}))
    return vm, display
end

local function test_reset_and_framebuffer()
    local vm, display = machine({0x00e0})
    check(vm.pc == Chip8.PROGRAM_ADDRESS, "program counter did not reset")
    check(vm.i == 0 and vm.sp == 0 and vm.delay_timer == 0 and vm.sound_timer == 0,
          "processor state did not reset")
    check(vm:peek(Chip8.FONT_ADDRESS) == 0xf0, "font was not installed")
    check(vm:peek(Chip8.FONT_ADDRESS + 79) == 0x80, "font tail was not installed")

    local was_dirty = display:consume_dirty()
    check(was_dirty, "new framebuffer was not dirty")
    check(not display:consume_dirty(), "framebuffer dirty flag did not clear")
    check(not display:xor_pixel(4, 5), "drawing an empty pixel reported a collision")
    check(display:xor_pixel(4, 5), "erasing a lit pixel did not report a collision")
    check(display:get_pixel(4, 5) == 0 and display:count_lit() == 0, "framebuffer XOR was incorrect")

    local snapshot = display:snapshot()
    check(snapshot.width == 64 and snapshot.height == 32 and snapshot.revision == 2,
          "framebuffer snapshot metadata was incorrect")
end

local function test_arithmetic_and_logic()
    local vm = machine({
        0x60fe, 0x6102, 0x8014, 0x8015, 0x8017, 0x8011, 0x8012, 0x8013,
        0x6203, 0x8206, 0x6280, 0x820e, 0x60ff, 0x7002, 0x8100
    })

    vm:run(2)
    vm:step()
    check(vm.v[0] == 0 and vm.v[0xf] == 1, "8XY4 carry was incorrect")
    vm:step()
    check(vm.v[0] == 0xfe and vm.v[0xf] == 0, "8XY5 borrow was incorrect")
    vm:step()
    check(vm.v[0] == 4 and vm.v[0xf] == 0, "8XY7 reverse subtraction was incorrect")
    vm:step()
    check(vm.v[0] == 6, "8XY1 OR was incorrect")
    vm:step()
    check(vm.v[0] == 2, "8XY2 AND was incorrect")
    vm:step()
    check(vm.v[0] == 0, "8XY3 XOR was incorrect")
    vm:run(2)
    check(vm.v[2] == 1 and vm.v[0xf] == 1, "8XY6 shift was incorrect")
    vm:run(2)
    check(vm.v[2] == 0 and vm.v[0xf] == 1, "8XYE shift was incorrect")
    vm:run(2)
    check(vm.v[0] == 1, "7XNN wrapping addition was incorrect")
    vm:step()
    check(vm.v[1] == 1, "8XY0 assignment was incorrect")
end

local function test_flow_control()
    local vm = machine({
        0x6001, 0x3001, 0x6000, 0x4001, 0x220e, 0x120c, 0x120c, 0x7001, 0x00ee
    })
    vm:step()
    vm:step()
    check(vm.pc == 0x206, "3XNN did not skip")
    vm:step()
    check(vm.pc == 0x208, "4XNN skipped on equality")
    vm:step()
    check(vm.pc == 0x20e and vm.sp == 1, "2NNN call was incorrect")
    vm:step()
    vm:step()
    check(vm.v[0] == 2 and vm.pc == 0x20a and vm.sp == 0, "00EE return was incorrect")
    vm:step()
    check(vm.pc == 0x20c, "1NNN jump was incorrect")

    vm = machine({0x6001, 0x6101, 0x5010, 0x6000, 0x6102, 0x9010, 0x6000})
    vm:run(3)
    check(vm.pc == 0x208, "5XY0 did not skip equal registers")
    vm:run(2)
    check(vm.pc == 0x20e, "9XY0 did not skip unequal registers")

    vm = machine({0x6004, 0xb220})
    vm:run(2)
    check(vm.pc == 0x224, "BNNN V0-relative jump was incorrect")

    vm = machine({0x0123, 0x6001})
    vm:run(2)
    check(vm.v[0] == 1, "0NNN compatibility no-op did not advance")
end

local function test_memory_and_fonts()
    local vm = machine({
        0xa300, 0x607b, 0xf033,
        0x600a, 0x610b, 0x620c, 0xa310, 0xf255,
        0x6000, 0x6100, 0x6200, 0xa310, 0xf265,
        0x630a, 0xf329, 0x6301, 0xafff, 0xf31e
    })
    vm:run(3)
    check(vm:peek(0x300) == 1 and vm:peek(0x301) == 2 and vm:peek(0x302) == 3,
          "FX33 BCD conversion was incorrect")
    vm:run(5)
    check(vm:peek(0x310) == 0x0a and vm:peek(0x311) == 0x0b and vm:peek(0x312) == 0x0c,
          "FX55 register store was incorrect")
    check(vm.i == 0x310, "modern FX55 unexpectedly changed I")
    vm:run(5)
    check(vm.v[0] == 0x0a and vm.v[1] == 0x0b and vm.v[2] == 0x0c,
          "FX65 register load was incorrect")
    check(vm.i == 0x310, "modern FX65 unexpectedly changed I")
    vm:run(2)
    check(vm.i == Chip8.FONT_ADDRESS + 50, "FX29 font lookup was incorrect")
    vm:run(3)
    check(vm.i == 0, "FX1E did not wrap the 12-bit index")

    vm = machine({0xa300, 0x6001, 0x6102, 0xf155}, {profile = "original"})
    vm:run(4)
    check(vm.i == 0x302, "original FX55 did not increment I")
end

local function test_input_and_timers()
    local vm = machine({0xf10a, 0x6201})
    vm:step()
    check(vm.pc == 0x200 and vm.v[1] == 0, "FX0A did not wait for a key")
    vm:set_key(5, true)
    vm:step()
    check(vm.pc == 0x202 and vm.v[1] == 5, "FX0A did not capture the pressed key")

    vm = machine({0x600a, 0xe09e, 0x6101, 0x6102})
    vm:step()
    vm:set_key(0x0a, true)
    vm:run(2)
    check(vm.v[1] == 2, "EX9E did not skip for a pressed key")

    vm = machine({0x600a, 0xe0a1, 0x6101, 0x6102})
    vm:run(3)
    check(vm.v[1] == 2, "EXA1 did not skip for a released key")

    vm = machine({0x6003, 0xf015, 0xf018, 0xf007})
    vm:run(4)
    check(vm.v[0] == 3 and vm.delay_timer == 3 and vm.sound_timer == 3,
          "timer instructions were incorrect")
    local was_sounding, still_sounding = vm:tick_timers(2)
    check(was_sounding and still_sounding and vm.delay_timer == 1 and vm.sound_timer == 1,
          "timer tick did not decrement")
    was_sounding, still_sounding = vm:tick_timers(2)
    check(was_sounding and not still_sounding and vm.delay_timer == 0 and vm.sound_timer == 0,
          "timers did not stop at zero")
end

local function test_random_and_quirks()
    local vm = machine({0xc00f}, {random_byte = function()
        return 0xab
    end})
    vm:step()
    check(vm.v[0] == 0x0b, "CXNN random mask was incorrect")

    vm = machine({0x6104, 0x6203, 0x8126})
    vm:run(3)
    check(vm.v[1] == 2 and vm.v[0xf] == 0, "modern shift source was incorrect")

    vm = machine({0x6104, 0x6203, 0x8126}, {profile = "original"})
    vm:run(3)
    check(vm.v[1] == 1 and vm.v[0xf] == 1, "original shift source was incorrect")

    vm = machine({0x6001, 0x6204, 0xb220}, {quirks = {jump_uses_vx = true}})
    vm:run(3)
    check(vm.pc == 0x224, "BXNN quirk did not use VX")
end

local function test_drawing()
    local vm, display = machine({0xa300, 0x603e, 0x611f, 0xd011, 0xd011, 0x00e0})
    vm:poke(0x300, 0xf0)
    vm:run(4)
    check(vm.v[0xf] == 0 and display:count_lit() == 4, "DXYN did not draw four pixels")
    check(display:get_pixel(62, 31) == 1 and display:get_pixel(63, 31) == 1 and
          display:get_pixel(0, 31) == 1 and display:get_pixel(1, 31) == 1,
          "DXYN did not wrap at the display edge")
    vm:step()
    check(vm.v[0xf] == 1 and display:count_lit() == 0, "DXYN collision was incorrect")
    display:xor_pixel(3, 3)
    vm:step()
    check(display:count_lit() == 0, "00E0 did not clear the display")

    vm, display = machine({0xa300, 0x603e, 0x611f, 0xd011}, {quirks = {clip_sprites = true}})
    vm:poke(0x300, 0xf0)
    vm:run(4)
    check(display:count_lit() == 2 and display:get_pixel(62, 31) == 1 and display:get_pixel(63, 31) == 1,
          "sprite clipping quirk was incorrect")
end

local function test_faults()
    expect_error("display", function()
        Chip8.new({display = {}})
    end)
    expect_error("unknown CHIP-8 quirk", function()
        machine({}, {quirks = {mystery = true}})
    end)
    expect_error("unsupported CHIP-8 opcode", function()
        machine({0xf0ff}):step()
    end)
    expect_error("stack underflow", function()
        machine({0x00ee}):step()
    end)
    expect_error("stack overflow", function()
        machine({0x2200}):run(17)
    end)
    expect_error("does not fit", function()
        local vm = machine({})
        vm:load_rom(string.rep("\0", Chip8.MEMORY_SIZE - Chip8.PROGRAM_ADDRESS + 1))
    end)
    expect_error("memory write outside", function()
        machine({0xafff, 0x60ff, 0xf033}):run(3)
    end)
    expect_error("random byte", function()
        machine({0xc0ff}, {random_byte = function()
            return 256
        end}):step()
    end)
    expect_error("key", function()
        machine({}):set_key(16, true)
    end)
    expect_error("instruction fetch outside", function()
        machine({0x1fff}):run(2)
    end)
end

local function test_device_adapter()
    local draw_count = 0
    local repaint_count = 0
    local drawn_boxes = {}
    local drawn_text = {}
    local drawn_text_y = {}
    local callbacks = {}
    local errors = {}
    local messages = {}
    local configured_rom_path = ""

    SEC = 1000000000000
    get_string_param = function(name)
        check(name == "ROM", "device requested an unexpected model property")
        return configured_rom_path
    end
    systime = function()
        return 5000
    end
    set_callback = function(time, event_id)
        callbacks[#callbacks + 1] = {time = time, event_id = event_id}
    end
    vsm_error = function(message)
        errors[#errors + 1] = message
    end
    print = function(message)
        messages[#messages + 1] = tostring(message)
    end

    graphics = {
        BLACK = 0,
        WHITE = 1,
        GREY = 2,
        BRIGHTWHITE = 3,
        BRIGHTGREEN = 4,
        TXJ_LEFT = 0x01,
        TXJ_CENTRE = 0x02,
        TXJ_RIGHT = 0x04,
        TXJ_MIDDLE = 0x08,
        ACF_LEFT = 0x01,
        set_draw_scale = function() end,
        set_pen_width = function() end,
        set_text_size = function() end,
        set_pen_colour = function() end,
        set_brush_colour = function() end,
        set_text_colour = function() end,
        draw_box = function(left, bottom, right, top)
            draw_count = draw_count + 1
            drawn_boxes[#drawn_boxes + 1] = {left = left, bottom = bottom, right = right, top = top}
        end,
        draw_text = function(_, y, _, _, text)
            draw_count = draw_count + 1
            drawn_text[#drawn_text + 1] = text
            drawn_text_y[text] = y
        end,
        repaint = function()
            repaint_count = repaint_count + 1
        end
    }

    dofile(CHIP8_DEVICE_PATH)
    check(type(device_pins) == "table" and #device_pins == 0,
          "device adapter unexpectedly declared electrical pins")
    check(type(device_init) == "function" and type(device_simulate) == "function",
          "device lifecycle callbacks were not defined")
    check(type(timer_callback) == "function" and type(device_graphics_init) == "function" and
          type(device_graphics_plot) == "function" and type(device_graphics_actuate) == "function",
          "device timer or graphics callbacks were not defined")

    device_init()
    device_graphics_init()
    check(repaint_count == 1, "graphics initialization did not request the first repaint")
    check(#callbacks == 1 and callbacks[1].time > systime(), "device did not schedule its first timer tick")
    check(#messages == 1 and messages[1]:find("DEMO", 1, true) ~= nil,
          "device did not report the built-in demo")

    draw_count = 0
    drawn_boxes = {}
    drawn_text_y = {}
    device_graphics_plot(0)
    local empty_display_draw_count = draw_count
    check(empty_display_draw_count >= 20, "device adapter did not draw the console face")
    check(drawn_boxes[2].bottom == 40 and drawn_boxes[2].top == 136,
          "device adapter did not convert the screen bounds to Proteus Y-up coordinates")
    check(drawn_boxes[3].bottom == 112 and drawn_boxes[3].top == 136,
          "device adapter did not place the first keypad row at the top")
    check(drawn_boxes[18].bottom == 28 and drawn_boxes[18].top == 52,
          "device adapter did not place the last keypad row at the bottom")
    check(drawn_text_y["CHIP-8"] == 152 and drawn_text_y["DEMO"] == 152,
          "device adapter did not place the title at the top")
    local status_y
    for _, text in ipairs(drawn_text) do
        if text:match("^RUN%s") then
            status_y = drawn_text_y[text]
        end
    end
    check(status_y == 22, "device adapter did not place the run status at the bottom")

    for index = 1, 60 do
        local scheduled = callbacks[index]
        check(scheduled ~= nil, "device stopped scheduling timer ticks")
        timer_callback(scheduled.time, scheduled.event_id)
    end
    check(#callbacks == 61, "device did not maintain its 60 Hz timer chain")
    check(callbacks[61].time - callbacks[1].time == SEC,
          "device timer accumulated drift over one second")
    check(repaint_count > 0, "framebuffer changes did not request a repaint")

    draw_count = 0
    drawn_text = {}
    device_graphics_plot(0)
    check(draw_count > empty_display_draw_count + 20, "built-in demo did not render framebuffer pixels")
    local status_text = table.concat(drawn_text, " ")
    check(status_text:find("DEMO", 1, true) ~= nil and status_text:find("RUN", 1, true) ~= nil,
          "device did not render its ROM and run status")

    local repaint_before_key = repaint_count
    check(device_graphics_actuate(0, 222, 124, graphics.ACF_LEFT),
          "device adapter did not accept a keypad click")
    check(repaint_count == repaint_before_key + 1, "keypad click did not request a repaint")
    timer_callback(callbacks[61].time, callbacks[61].event_id)
    check(repaint_count > repaint_before_key + 1, "keypad pulse was not released on the next timer tick")
    check(device_graphics_actuate(string.byte("x"), 0, 0, 0),
          "device adapter did not accept the conventional keyboard layout")
    check(not device_graphics_actuate(0, 4, 4, graphics.ACF_LEFT),
          "device adapter accepted a click outside the keypad")

    configured_rom_path = "__openvsm_missing_chip8_rom__.ch8"
    local callback_count = #callbacks
    device_init()
    check(#callbacks == callback_count, "device scheduled execution after a ROM load failure")
    check(#errors == 1 and errors[1]:find(configured_rom_path, 1, true) ~= nil,
          "device did not report the failing per-instance ROM path")
    drawn_text = {}
    device_graphics_plot(0)
    check(table.concat(drawn_text, " "):find("ROM ERROR", 1, true) ~= nil,
          "device did not display its ROM error state")
end

test_reset_and_framebuffer()
test_arithmetic_and_logic()
test_flow_control()
test_memory_and_fonts()
test_input_and_timers()
test_random_and_quirks()
test_drawing()
test_faults()
test_device_adapter()

assert(checks >= 120, "CHIP-8 test coverage unexpectedly shrank")
