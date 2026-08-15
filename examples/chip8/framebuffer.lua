local Framebuffer = {}
Framebuffer.__index = Framebuffer

local function check_coordinate(name, value, maximum)
    if math.type(value) ~= "integer" or value < 0 or value >= maximum then
        error(string.format("%s must be an integer from 0 to %d", name, maximum - 1), 3)
    end
end

function Framebuffer.new(width, height)
    if math.type(width) ~= "integer" or width <= 0 then
        error("framebuffer width must be a positive integer", 2)
    end
    if math.type(height) ~= "integer" or height <= 0 then
        error("framebuffer height must be a positive integer", 2)
    end

    local display = setmetatable({
        dirty = true,
        height = height,
        pixels = {},
        revision = 0,
        width = width
    }, Framebuffer)
    for index = 1, width * height do
        display.pixels[index] = 0
    end
    return display
end

function Framebuffer:index(x, y)
    check_coordinate("x", x, self.width)
    check_coordinate("y", y, self.height)
    return y * self.width + x + 1
end

function Framebuffer:clear()
    local changed = false
    for index = 1, #self.pixels do
        if self.pixels[index] ~= 0 then
            self.pixels[index] = 0
            changed = true
        end
    end
    if changed then
        self.dirty = true
        self.revision = self.revision + 1
    end
end

function Framebuffer:xor_pixel(x, y)
    local index = self:index(x, y)
    local collision = self.pixels[index] ~= 0
    self.pixels[index] = collision and 0 or 1
    self.dirty = true
    self.revision = self.revision + 1
    return collision
end

function Framebuffer:get_pixel(x, y)
    return self.pixels[self:index(x, y)]
end

function Framebuffer:count_lit()
    local count = 0
    for _, pixel in ipairs(self.pixels) do
        count = count + pixel
    end
    return count
end

function Framebuffer:snapshot()
    local pixels = {}
    for index, pixel in ipairs(self.pixels) do
        pixels[index] = pixel
    end
    return {
        height = self.height,
        pixels = pixels,
        revision = self.revision,
        width = self.width
    }
end

function Framebuffer:consume_dirty()
    local was_dirty = self.dirty
    self.dirty = false
    return was_dirty, self.revision
end

return Framebuffer
