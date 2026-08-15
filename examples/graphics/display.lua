device_pins = {}

local value = 0

function device_init()
end

function device_simulate()
end

function device_graphics_init()
    graphics.set_draw_scale(96)
    graphics.set_pen_width(1)
    graphics.set_text_size(14)
    graphics.set_text_colour(graphics.BRIGHTGREEN)
end

function device_graphics_plot(state)
    graphics.set_pen_colour(graphics.BRIGHTGREEN)
    graphics.set_brush_colour(graphics.BLACK)
    graphics.draw_box(0, 0, 160, 60)
    graphics.draw_text(80, 30, 0, graphics.TXJ_CENTRE | graphics.TXJ_MIDDLE, string.format("%02X", value))
end

function device_graphics_animate(element, data_type, new_value)
    if data_type == graphics.ADT_INTEGER then
        value = new_value & 0xff
        graphics.repaint(false)
    end
end

function device_graphics_actuate(key, x, y, flags)
    if (flags & graphics.ACF_LEFT) ~= 0 then
        value = (value + 1) & 0xff
        graphics.repaint(false)
        return true
    end
    return false
end
