-- A 6850-compatible ACIA, modelled in Lua.
--
-- The point of it is visibility. The Proteus MC6850 primitive is a black box,
-- and when a UART misbehaves the terminal only ever shows garbage, which fits
-- a dozen explanations equally well. This model can log its own frames, so a
-- wrong rate or format shows up as a number instead of a guess. It was a frame
-- log from this model that exposed the real fault on this board: test code
-- writing progress markers to ports F0-F4, inside the ACIA's decoded range,
-- which rewrote the control register after every character. Most of what had
-- looked like quirks of the Proteus part came from that, so the stock MC6850
-- may well be usable here; it has not been retried since the fix.
--
-- This model is driven by the two Z80 strobes rather than a 6800 enable clock,
-- and takes its bit timing from a clock pin, so the rate depends on that pin
-- and the divider and on nothing else. The register layout and semantics
-- follow the 6850 datasheet, so a ROM written for the real part runs
-- unchanged. NASCOM BASIC is the case this was built for.
--
-- It deliberately reuses the pin names on the stock MC6850 symbol, so the
-- existing part can be pointed at this model by changing two properties and
-- moving one wire. Two pins are used for something other than their printed
-- meaning, which is the price of not having to draw a new symbol:
--
--   D[0..7]  data bus      resolved as D0 through D7 out of the bus pin
--   RS       A0            register select: 0 control and status, 1 data
--   CS0      A7            selects the 0x80 to 0xFF port range
--   CS1      M1            high for ordinary I/O, low for interrupt acknowledge
--   /CS2     IORQ          so it only answers I/O cycles
--   E        RD            read strobe. Not a 6800 enable clock.
--   R//W     WR            write strobe. Low means write, as the name implies.
--   RXCLK    clock gen     16 or 64 times the wanted baud rate
--   RXDATA   terminal TXD
--   TXDATA   terminal RXD
--   /IRQ     INT           pulled low while an enabled interrupt is pending

local FLOATING = FLT

device_pins = {}

local function declare(name, on_time, off_time)
    device_pins[#device_pins + 1] = {name = name, on_time = on_time or 0, off_time = off_time or 0}
    return name
end

-- Active-low pins accept both spellings, plain and overbarred, the same way
-- the Z80 model does, so the symbol can draw them either way.
local function either(name) return name .. ",$" .. name .. "$" end

local DATA_NAMES = {}
for bit = 0, 7 do DATA_NAMES[bit] = declare(string.format("D%d", bit), 0, 0) end

local RS_NAME  = declare("RS")
local CS0_NAME = declare("CS0")
local CS1_NAME = declare("CS1")
local CS2_NAME = declare(either("CS2"))
local RD_NAME  = declare("E")
local WR_NAME  = declare("R/W,R/$W$")
local CLK_NAME = declare("RXCLK")
local RXD_NAME = declare("RXDATA")
local TXD_NAME = declare("TXDATA", 0, 0)
local IRQ_NAME = declare(either("IRQ"), 0, 0)

local data_pins = {}
local rs, cs0, cs1, cs2, rd, wr, clk, rxd, txd, irq

-- Control register fields, straight from the datasheet.
local DIVIDE = {[0] = 1, [1] = 16, [2] = 64, [3] = 0}   -- 3 means master reset
local WORD = {
    [0] = {bits = 7, parity = "even", stop = 2},
    [1] = {bits = 7, parity = "odd",  stop = 2},
    [2] = {bits = 7, parity = "even", stop = 1},
    [3] = {bits = 7, parity = "odd",  stop = 1},
    [4] = {bits = 8, parity = nil,    stop = 2},
    [5] = {bits = 8, parity = nil,    stop = 1},
    [6] = {bits = 8, parity = "even", stop = 1},
    [7] = {bits = 8, parity = "odd",  stop = 1},
}

local control, divide, word
local tdr, tdr_full             -- transmit data register
local tx_bits, tx_index         -- the frame currently being shifted out
local tx_tick                   -- clock ticks within the current bit
local rdr, rdrf, overrun, framing
local rx_state, rx_tick, rx_bit, rx_shift
local driving                   -- the byte this model is putting on the bus

local TRACE = false
local function log(...) if TRACE then print(string.format(...)) end end

-- Diagnostic: how many clock ticks actually elapse per character. At 16 ticks
-- a bit and ten bits a character this should read 160. Anything else means the
-- clock pin is not ticking at the rate the schematic says it is.
local ticks, last_frame_tick = 0, 0

local function selected()
    return cs0:ishigh() and cs1:ishigh() and cs2:islow()
end

local function parity_of(value, bits, kind)
    local ones = 0
    for i = 0, bits - 1 do ones = ones + ((value >> i) & 1) end
    if kind == "even" then return ones % 2 end
    return 1 - (ones % 2)
end

-- Build the bit sequence for one character: start bit, data least significant
-- first, optional parity, then the stop bits. Holding it as a list keeps the
-- shifting trivial and lets the format fields fall straight out of the table.
local function frame(value)
    local bits = {0}
    for i = 0, word.bits - 1 do bits[#bits + 1] = (value >> i) & 1 end
    if word.parity then bits[#bits + 1] = parity_of(value, word.bits, word.parity) end
    for _ = 1, word.stop do bits[#bits + 1] = 1 end
    return bits
end

local function master_reset()
    tdr, tdr_full = 0, false
    tx_bits, tx_index, tx_tick = nil, 0, 0
    rdr, rdrf, overrun, framing = 0, false, false, false
    rx_state, rx_tick, rx_bit, rx_shift = "idle", 0, 0, 0
    if txd then txd:set(1) end
end

local function irq_pending()
    local rx_int = (control & 0x80) ~= 0 and rdrf
    local tx_int = ((control >> 5) & 3) == 1 and not tdr_full
    return rx_int or tx_int
end

local function update_irq()
    if irq_pending() then irq:set(0) else irq:setstate(FLOATING) end
end

local function status()
    local s = 0
    if rdrf then s = s | 0x01 end
    if not tdr_full then s = s | 0x02 end
    -- DCD and CTS read as low on this board, so their bits stay clear.
    if framing then s = s | 0x10 end
    if overrun then s = s | 0x20 end
    if irq_pending() then s = s | 0x80 end
    return s
end

local function release_bus()
    if driving == nil then return end
    for bit = 0, 7 do data_pins[bit]:setstate(FLOATING) end
    driving = nil
end

local function drive_bus(value)
    for bit = 0, 7 do data_pins[bit]:set((value >> bit) & 1) end
    driving = value
end

local function bus_byte()
    local value = 0
    for bit = 0, 7 do
        if data_pins[bit]:ishigh() then value = value | (1 << bit) end
    end
    return value
end

local function write_control(value)
    control = value
    local d = DIVIDE[value & 3]
    if d == 0 then
        log("acia: master reset\n")
        control = 0
        master_reset()
        divide, word = 16, WORD[5]
        update_irq()
        return
    end
    divide = d
    word = WORD[(value >> 2) & 7]
    log("acia: control %02X - %d data, %s parity, %d stop, divide %d\n",
        value, word.bits, word.parity or "no", word.stop, divide)
    update_irq()
end

local function on_read()
    if not selected() or rd:ishigh() then
        release_bus()
        return
    end
    if rs:ishigh() then
        drive_bus(rdr)
        rdrf, overrun, framing = false, false, false
        update_irq()
    else
        drive_bus(status())
    end
end

-- The Z80 model publishes its data before it asserts WR, so the byte is already
-- valid on the falling edge and this does not have to race the release.
local function on_write()
    if not selected() or wr:ishigh() then return end
    local value = bus_byte()
    if rs:ishigh() then
        tdr, tdr_full = value, true
        update_irq()
    else
        write_control(value)
    end
end

-- One tick of the baud clock. Every piece of bit timing lives here, so the rate
-- depends only on this pin and the divider, never on what the CPU is doing.
local function on_clock()
    if not clk:ishigh() then return end

    ticks = ticks + 1
    tx_tick = tx_tick + 1
    if tx_tick >= divide then
        tx_tick = 0
        if tx_bits then
            tx_index = tx_index + 1
            if tx_index > #tx_bits then
                tx_bits = nil
                txd:set(1)
            else
                txd:set(tx_bits[tx_index])
            end
        end
        if tx_bits == nil and tdr_full then
            log("acia: frame %02X starts, %d ticks since the last, divide %d, %d bits\n",
                tdr, ticks - last_frame_tick, divide, #frame(tdr))
            last_frame_tick = ticks
            tx_bits, tx_index = frame(tdr), 1
            txd:set(tx_bits[1])
            tdr_full = false
            update_irq()
        end
    end

    -- Receiver: an ordinary oversampling front end. With a divider of 1 there
    -- is no oversampling to be had, which is true of the real part as well.
    local half = divide // 2
    if rx_state == "idle" then
        if rxd:islow() then
            rx_state, rx_tick, rx_bit, rx_shift = "start", 0, 0, 0
        end
    else
        rx_tick = rx_tick + 1
        if rx_state == "start" then
            if rx_tick >= half then
                if rxd:islow() then
                    rx_state, rx_tick = "data", 0
                else
                    rx_state = "idle"           -- noise, not a real start bit
                end
            end
        elseif rx_state == "data" then
            if rx_tick >= divide then
                rx_tick = 0
                if rxd:ishigh() then rx_shift = rx_shift | (1 << rx_bit) end
                rx_bit = rx_bit + 1
                if rx_bit >= word.bits then rx_state = "stop" end
            end
        elseif rx_state == "stop" then
            if rx_tick >= divide then
                if rxd:islow() then framing = true end
                if rdrf then overrun = true end
                rdr, rdrf = rx_shift, true
                rx_state = "idle"
                update_irq()
                log("acia: received %02X\n", rx_shift)
            end
        end
    end
end

local function pin(name)
    local object = _G[name]
    if object == nil then
        error(string.format("Proteus registered no pin for %s", name), 0)
    end
    return object
end

function device_init()
    for bit = 0, 7 do data_pins[bit] = pin(DATA_NAMES[bit]) end
    rs, cs0, cs1, cs2 = pin(RS_NAME), pin(CS0_NAME), pin(CS1_NAME), pin(CS2_NAME)
    rd, wr, clk = pin(RD_NAME), pin(WR_NAME), pin(CLK_NAME)
    rxd, txd, irq = pin(RXD_NAME), pin(TXD_NAME), pin(IRQ_NAME)

    control, divide, word = 0, 16, WORD[5]
    driving = nil
    master_reset()
    release_bus()
    update_irq()

    rd:onchange(on_read)
    wr:onchange(on_write)
    cs2:onchange(function() if not selected() then release_bus() end end)
    clk:onchange(on_clock)

    print("acia: 6850-compatible ACIA ready\n")
end

function device_simulate()
end
