device_pins = {}

local BODY_WIDTH = 344
local BODY_HEIGHT = 166
local SCREEN_LEFT = 12
local SCREEN_TOP = 30
local SCREEN_RIGHT = 204
local SCREEN_BOTTOM = 126
local KEYPAD_LEFT = 220
local KEYPAD_TOP = 30
local KEY_SIZE = 24
local KEY_GAP = 4

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

local selected_key

local function key_bounds(index)
    local zero_based = index - 1
    local column = zero_based % 4
    local row = zero_based // 4
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
    graphics.draw_text((left + right) // 2, (top + bottom) // 2, 0,
                       graphics.TXJ_CENTRE | graphics.TXJ_MIDDLE, key.label)
end

function device_init()
end

function device_simulate()
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
    graphics.draw_text(BODY_WIDTH - 12, 14, 0, graphics.TXJ_RIGHT | graphics.TXJ_MIDDLE,
                       "HOST STUB")

    graphics.set_pen_colour(graphics.BRIGHTGREEN)
    graphics.set_brush_colour(graphics.BLACK)
    graphics.draw_box(SCREEN_LEFT, SCREEN_TOP, SCREEN_RIGHT, SCREEN_BOTTOM)
    graphics.set_text_colour(graphics.BRIGHTGREEN)
    graphics.set_text_size(14)
    graphics.draw_text((SCREEN_LEFT + SCREEN_RIGHT) // 2,
                       (SCREEN_TOP + SCREEN_BOTTOM) // 2, 0,
                       graphics.TXJ_CENTRE | graphics.TXJ_MIDDLE, "NO ROM")

    graphics.set_text_size(10)
    graphics.set_text_colour(graphics.BRIGHTWHITE)
    graphics.draw_text(SCREEN_LEFT, 144, 0, graphics.TXJ_LEFT | graphics.TXJ_MIDDLE,
                       "STOP  PC:0200  60 Hz")

    for index, key in ipairs(keys) do
        draw_key(index, key)
    end
end

function device_graphics_animate(_, _, _)
end

function device_graphics_actuate(_, x, y, flags)
    if (flags & graphics.ACF_LEFT) == 0 then
        return false
    end

    for index, key in ipairs(keys) do
        local left, top, right, bottom = key_bounds(index)
        if x >= left and x <= right and y >= top and y <= bottom then
            selected_key = key.value
            graphics.repaint(false)
            return true
        end
    end

    return false
end
