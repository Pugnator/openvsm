-- A real Z80, run by the Z80core library, wired to a Proteus schematic.
--
-- The core holds no memory, no devices and no scheduler. It is clocked from
-- outside and asks for bytes through its pins, and everything it asks for is
-- answered by the schematic: an EPROM, a RAM, an output latch. This script is
-- only the adapter between the two, and deliberately knows nothing about the
-- memory map - that lives in the address decoding, where it belongs.
--
-- Which makes this a test of the CPU core rather than a demonstration built
-- around it. If a machine cycle is the wrong length, releases the data bus at
-- the wrong moment, or drives an address the decoder does not expect, real
-- memory notices and the program stops working.
--
-- z80core.dll must sit beside this file. See README.md.

-- NSEC and FLT come from the OpenVSM host, which registers its constants
-- before this chunk runs. The fallbacks keep the script readable outside it.
local PICOSECONDS_PER_NS = NSEC or 1000
local FLOATING = FLT

-- Propagation delays. The core sets the refresh address in the same step that
-- it drops MREQ and RD, so both reach the host on one edge. Equal delays land
-- them at the same instant in Proteus's event queue, which leaves the order
-- undefined and makes the publish order in the clock handler meaningless. Two
-- different delays give a deterministic order: strobes first, address after.
--
-- The size of the gap turns out not to matter. Widening it from 50 ns to 210 ns
-- produced a byte-for-byte identical bus trace, so nothing downstream is racing
-- against it. Only the ordering is load-bearing; the numbers just have to
-- differ and stay well inside a half clock period, since the address is issued
-- one edge before the strobes that use it.
local OUTPUT_DELAY = 40 * PICOSECONDS_PER_NS
local ADDRESS_DELAY = 250 * PICOSECONDS_PER_NS

-- How many bus cycles TRACE reports before it goes quiet. A trace is for
-- seeing how the machine starts, not for logging it forever.
local TRACE_LIMIT = 20000

-- How many bad reads to name before the model goes quiet. A contended bus
-- repeats, and one line per occurrence is what separates a single glitch
-- from a driver that answers every cycle of a given shape.
local BAD_READ_REPORTS = 20

local function script_directory()
    local source = debug.getinfo(1, "S").source
    if source:sub(1, 1) == "@" then
        source = source:sub(2)
    end
    local directory = source:match("^(.*[\\/])")
    assert(directory ~= nil, "the Z80 device script directory is unavailable")
    return directory
end

local DIRECTORY = script_directory()

-- OpenVSM leaves package.cpath alone and Proteus runs from the project
-- directory, so require() is pointed at this script's own directory before the
-- binding is asked for. A failure here is reported from device_init rather
-- than thrown, so the component still registers its pins and says why.
package.cpath = DIRECTORY .. "?.dll;" .. package.cpath

local core, core_error
do
    local loaded, result = pcall(require, "z80core")
    if loaded then
        core = result
    else
        core_error = tostring(result)
    end
end

-- ---------------------------------------------------------------------------
-- Pin declarations
-- ---------------------------------------------------------------------------

device_pins = {}

local function declare(name, on_time, off_time)
    device_pins[#device_pins + 1] = {name = name, on_time = on_time or 0, off_time = off_time or 0}
    return name
end

-- Proteus resolves a pin from a comma-separated list of alternatives, so every
-- active-low pin accepts both the plain name and the overbarred form a Z80
-- symbol normally carries. OpenVSM uses the same string as the pin's Lua
-- global name, which is why this script looks its pins up through _G instead
-- of naming them directly.
local function either_spelling(name)
    return name .. ",$" .. name .. "$"
end

local ADDRESS_NAMES = {}
for bit = 0, 15 do
    ADDRESS_NAMES[bit] = declare(string.format("A%d", bit), ADDRESS_DELAY, ADDRESS_DELAY)
end

-- The data pins are declared with no transition delay, unlike the address and
-- control pins. OpenVSM gives a driven transition the pin's on_time/off_time
-- but a float none at all (pin_timing.cc), so a bus that is driven with a
-- delay and released without one has its release land ahead of the drive it
-- follows. Zero on both sides keeps the turnaround ordered.
local DATA_NAMES = {}
for bit = 0, 7 do
    DATA_NAMES[bit] = declare(string.format("D%d", bit), 0, 0)
end

-- Control outputs. Every one is active low on the part and "asserted is true"
-- in the core, so publishing inverts.
local CONTROL_OUTPUTS = {
    {field = "M1"}, {field = "MREQ"}, {field = "IORQ"}, {field = "RD"},
    {field = "WR"}, {field = "RFSH"}, {field = "HALT"}, {field = "BUSAK"}
}
for _, entry in ipairs(CONTROL_OUTPUTS) do
    entry.declared = declare(either_spelling(entry.field), OUTPUT_DELAY, OUTPUT_DELAY)
end

-- Control inputs, also active low. The core reads these from its pin structure
-- as part of advancing, so they are kept current between edges.
local CONTROL_INPUTS = {
    {field = "WAIT"}, {field = "INT"}, {field = "NMI"}, {field = "RESET"}, {field = "BUSRQ"}
}
for _, entry in ipairs(CONTROL_INPUTS) do
    entry.declared = declare(either_spelling(entry.field))
end

local CLOCK_NAME = declare("CLK")

-- VCC and GND are on the symbol but are deliberately not declared: a digital
-- Proteus model has no use for its power pins.

-- ---------------------------------------------------------------------------
-- Runtime state
-- ---------------------------------------------------------------------------

local cpu
local clock
local address_pins = {}
local data_pins = {}

local running = false

-- What this model last drove, so a pin is only written when its level moves.
local driven_address
local driven_data

-- True while the core is reading and the data bus belongs to the schematic.
local sampling = false

-- The write strobe released last edge; the byte is held one edge, then let go.
local release_pending = false

-- An address change that lands on the same edge as a strobe release is the
-- one case the memories cannot survive. Ordering the two publishes does not
-- separate them enough: a gap in nanoseconds still falls inside the part's
-- access window, and the EPROM re-triggers a read while it is mid-release and
-- then holds the bus. Holding the address back one edge separates them by a
-- half clock period instead, so the strobes go inactive against a stable
-- address, exactly as they do at the end of an operand read, which works.
local address_pending = nil

local trace_enabled = false
local traced = 0
local open_cycle
local bad_reads_reported = 0

-- Cached masks, to keep the edge handler off the module table.
local M1, MREQ, IORQ, RD, WR, RFSH, BUSAK, TRANSFER, STROBES
local CHANGED_A, CHANGED_D

local function stop(message)
    running = false
    print("z80: stopped - " .. tostring(message) .. "\n")
    if type(vsm_error) == "function" then
        vsm_error("z80: " .. tostring(message))
    end
end

-- ---------------------------------------------------------------------------
-- The buses
-- ---------------------------------------------------------------------------

local function publish_address(value)
    local previous = driven_address
    if previous == value then
        return
    end

    for bit = 0, 15 do
        local level = (value >> bit) & 1
        if previous == nil or ((previous >> bit) & 1) ~= level then
            address_pins[bit]:set(level)
        end
    end

    driven_address = value
end

--- @param value integer|nil A byte to drive, or nil to release the bus.
local function publish_data(value)
    local previous = driven_data
    if previous == value then
        return
    end

    if value == nil then
        for bit = 0, 7 do
            data_pins[bit]:setstate(FLOATING)
        end
        driven_data = nil
        return
    end

    for bit = 0, 7 do
        local level = (value >> bit) & 1
        if previous == nil or ((previous >> bit) & 1) ~= level then
            data_pins[bit]:set(level)
        end
    end

    driven_data = value
end

--- Publish exactly the control pins the core says it drove this edge.
-- z80_tick() returns a mask of the outputs it moved, which is what the host is
-- meant to act on: "if (changed & Z80_MREQ) publish_mreq(...)".
local function publish_control(changed, ctrl)
    for index = 1, #CONTROL_OUTPUTS do
        local entry = CONTROL_OUTPUTS[index]
        if (changed & entry.mask) ~= 0 then
            -- Asserted in the core is a low pin on the part.
            entry.pin:set((ctrl & entry.mask) ~= 0 and 0 or 1)
        end
    end
end

--- What the schematic is currently driving onto D0..D7.
-- A pin that is neither high nor low reads as 0, so an unanswered read looks
-- like a bus full of NOPs rather than like a random program.
local function sample_data_bus()
    local value = 0
    for bit = 0, 7 do
        if data_pins[bit]:get() == 1 then
            value = value | (1 << bit)
        end
    end
    return value
end

-- ---------------------------------------------------------------------------
-- Tracing
-- ---------------------------------------------------------------------------

local function cycle_name(ctrl)
    if (ctrl & MREQ) ~= 0 then
        if (ctrl & RD) ~= 0 then
            return (ctrl & M1) ~= 0 and "fetch" or "read "
        elseif (ctrl & WR) ~= 0 then
            return "write"
        end
    elseif (ctrl & IORQ) ~= 0 then
        if (ctrl & M1) ~= 0 then
            return "intak"
        elseif (ctrl & RD) ~= 0 then
            return "in   "
        elseif (ctrl & WR) ~= 0 then
            return "out  "
        end
    end
    return nil
end

local function close_cycle(data)
    if open_cycle == nil then
        return
    end

    traced = traced + 1

    -- I/O cycles are never suppressed. On a machine whose only peripheral is a
    -- UART they are the conversation worth watching, there are few of them next
    -- to the fetch traffic, and the interesting ones happen long after the
    -- first two hundred cycles have gone by.
    if open_cycle.io or traced <= TRACE_LIMIT then
        print(string.format("z80: %s %04X %02X\n", open_cycle.name, open_cycle.address, data or 0))
    end

    if traced == TRACE_LIMIT then
        print(string.format("z80: trace limit of %d cycles reached; I/O cycles still follow\n", TRACE_LIMIT))
    end

    open_cycle = nil
end

-- ---------------------------------------------------------------------------
-- The clock edge
-- ---------------------------------------------------------------------------

local function on_clock_edge()
    if not running then
        return
    end

    -- A read is in flight: hand the core whatever the schematic is driving
    -- now. The core samples partway through the cycle, and by then the address
    -- has been out for a T-state or more, so the memory has long since
    -- answered - provided the clock is slow next to its access time.
    if sampling then
        local value = sample_data_bus()
        cpu:setdata(value)

        -- The value alone cannot tell a register holding zero from a bus nobody
        -- is driving, since an undriven pin reads as 0. So when the pins show
        -- nobody driving, or two drivers fighting, say so and say whether this
        -- model still holds the bus. The first few are reported, then it goes
        -- quiet; one fault usually repeats on every cycle of the same shape.
        if bad_reads_reported < BAD_READ_REPORTS then
            local contended, floating = 0, 0
            for bit = 0, 7 do
                if data_pins[bit]:iscontention() then contended = contended | (1 << bit) end
                if data_pins[bit]:isfloating() then floating = floating | (1 << bit) end
            end
            if contended ~= 0 or floating == 0xFF then
                bad_reads_reported = bad_reads_reported + 1
                print(string.format(
                    "z80: read %04X = %02X | contended D:%02X floating D:%02X | this model is %s\n",
                    driven_address or 0, value, contended, floating,
                    driven_data and string.format("driving %02X", driven_data) or "not driving"))
                if bad_reads_reported == BAD_READ_REPORTS then
                    print(string.format("z80: %d bad reads reported; quiet from here\n", BAD_READ_REPORTS))
                end
            end
        end
    end

    -- The level goes across as a boolean, not 0 or 1: z80core's binding tests
    -- its argument with lua_toboolean first, and the number 0 is true in Lua,
    -- so cpu:tick(0) would raise the clock instead of dropping it.
    local changed = cpu:tick(clock:ishigh())

    -- The write strobe released on the previous edge; the byte stayed on the
    -- bus through it, as the part holds data past WR, and is let go now -
    -- before the early return, because this is host bookkeeping and the core
    -- reporting nothing moved on this edge is exactly the usual case here.
    --
    -- The one-edge hold is also what makes the release survive OpenVSM's
    -- event timing: a driven pin transition carries OUTPUT_DELAY but a float
    -- carries none, so a float issued on the strobe's own edge would reach
    -- the schematic 60 ns before the strobe rise it was issued after - and a
    -- 62256 latches on that rise.
    if address_pending ~= nil then
        publish_address(address_pending)
        address_pending = nil
    end

    if release_pending then
        publish_data(nil)
        release_pending = false
    end

    if changed == 0 then
        -- Most edges drive nothing. The core says so, and the model skips both
        -- the pin diff and the direction decision rather than rediscovering it.
        return
    end

    local ctrl = cpu:ctrl()

    -- From here down the handler is the EMBEDDING.md host loop: publish what
    -- z80_tick() says it drove, on the edge it says it drove it. The sequence
    -- within a machine cycle - address at T1, write data at T2 rise, WR at T2
    -- fall, release at T3 fall with the byte still out - is the core's, and
    -- publishing per edge inherits it. The pin struct can express everything
    -- except high impedance (its own BUSAK comment: a struct owned by the
    -- caller has no way to say Z), so the data releases are the only
    -- host-added behaviour.

    -- Which strobes are going inactive on this edge. The address must not move
    -- while one is still asserted: the core sets the refresh address in the
    -- same step that it drops MREQ and RD, so publishing the address first
    -- leaves the refresh address on the bus with the fetch's strobes still
    -- live, and a ROM selected by A15=0 drives whatever sits at I:R. That byte
    -- then turns up in place of the memory the next cycle asks for.
    local releasing = (changed & STROBES & ~ctrl) ~= 0

    if releasing then
        publish_control(changed, ctrl)
    end

    if (changed & CHANGED_A) ~= 0 then
        if releasing then
            -- Let the strobes settle first; this goes out on the next edge.
            address_pending = cpu:addr()
        else
            publish_address(cpu:addr())
        end
    end

    -- CHANGED_D is the core saying "I drove the data bus to a new value" - it
    -- writes its D pin only when the CPU owns the bus. The WR clause covers
    -- the value not moving: a write whose byte equals whatever the last read
    -- left in the pin raises no CHANGED_D, but the strobe still names it a
    -- write. Both publishes go out before the strobes so data never trails WR.
    -- An interrupt acknowledge is a read even though RD is never asserted: M1
    -- and IORQ together, with the device putting its vector on the bus. The
    -- model must stay off the bus for it, or it fights whatever answers.
    local intack = (ctrl & IORQ) ~= 0 and (ctrl & M1) ~= 0

    if (ctrl & RD) == 0 and not intack then
        if (changed & CHANGED_D) ~= 0 then
            publish_data(cpu:data())
        elseif (changed & WR) ~= 0 and (ctrl & WR) ~= 0 then
            publish_data(cpu:data())
        end
    end

    if not releasing then
        publish_control(changed, ctrl)
    end

    if (changed & WR) ~= 0 and (ctrl & WR) == 0 then
        release_pending = true
    end

    -- "BUSAK asserted means these are not mine."
    if (ctrl & BUSAK) ~= 0 and driven_data ~= nil then
        publish_data(nil)
    end

    -- What the host must feed: a read wants the schematic's byte in the core's
    -- D pin before the sampling edge. RD names every memory and I/O read; M1
    -- with IORQ is the interrupt acknowledge, where the device supplies its
    -- vector with no strobe at all.
    local reading = (ctrl & RD) ~= 0 or intack
    if reading and not sampling then
        sampling_io = (ctrl & IORQ) ~= 0
    end
    sampling = reading

    if trace_enabled then
        local name = cycle_name(ctrl)
        if name ~= nil then
            if open_cycle == nil then
                open_cycle = {name = name, address = cpu:addr(), io = (ctrl & IORQ) ~= 0}
            end
        elseif open_cycle ~= nil then
            -- Whichever way the cycle went, the byte is in the core's data pin
            -- by now: it drove it on a write, and the sampling above fed it in
            -- on a read.
            close_cycle(cpu:data())
        end
    end
end

-- ---------------------------------------------------------------------------
-- Setup
-- ---------------------------------------------------------------------------

local function resolve(declared)
    local object = _G[declared]
    if object == nil then
        error(string.format("Proteus registered no pin for '%s'", declared), 0)
    end
    return object
end

local function configure()
    clock = resolve(CLOCK_NAME)

    for bit = 0, 15 do
        address_pins[bit] = resolve(ADDRESS_NAMES[bit])
    end
    for bit = 0, 7 do
        data_pins[bit] = resolve(DATA_NAMES[bit])
    end
    for _, entry in ipairs(CONTROL_OUTPUTS) do
        entry.pin = resolve(entry.declared)
        entry.mask = core[entry.field]
    end
    for _, entry in ipairs(CONTROL_INPUTS) do
        entry.pin = resolve(entry.declared)
        entry.mask = core[entry.field]
    end

    M1, MREQ, IORQ, RD, WR = core.M1, core.MREQ, core.IORQ, core.RD, core.WR
    RFSH = core.RFSH
    BUSAK = core.BUSAK
    TRANSFER = MREQ | IORQ
    STROBES = MREQ | IORQ | RD | WR
    CHANGED_A, CHANGED_D = core.CHANGED_A, core.CHANGED_D

    trace_enabled = get_bool_param("TRACE") and true or false

    cpu = core.new()
    cpu:reset()
end

function device_init()
    running = false
    driven_address, driven_data = nil, nil
    sampling = false
    traced, open_cycle = 0, nil
    bad_reads_reported = 0
    sampling_io = false
    release_pending = false
    address_pending = nil

    if core == nil then
        stop(string.format("z80core.dll was not loaded. Place it beside this script (%s) or on package.cpath. " ..
                           "require reported: %s", DIRECTORY, core_error))
        return
    end

    local configured, configuration_error = pcall(configure)
    if not configured then
        stop(configuration_error)
        return
    end

    -- A pin's change handler is what makes Proteus activate this model, so the
    -- clock and the control inputs are the only pins that carry one. The data
    -- bus deliberately does not: it is read at the clock edge that needs it,
    -- not chased as it settles.
    clock:onchange(on_clock_edge)

    for _, entry in ipairs(CONTROL_INPUTS) do
        local pin, mask = entry.pin, entry.mask
        pin:onchange(function()
            cpu:setctrl(mask, pin:islow())
        end)
        -- onchange remembers the state it was registered at and fires only on a
        -- change from it, so a pin already held low - a power-on reset holding
        -- RESET, say - would never be seen. Sample every one once, here.
        cpu:setctrl(mask, pin:islow())
    end

    running = true

    -- Publish the reset state: address out, every strobe released, data bus
    -- floating, so the schematic starts from something rather than nothing.
    publish_address(cpu:addr())
    -- Nothing has been published yet, so every control pin counts as changed.
    publish_control(0xFFFFFFFF, cpu:ctrl())
    for bit = 0, 7 do
        data_pins[bit]:setstate(FLOATING)
    end
    driven_data = nil

    print(string.format("z80: core %s loaded, fetching from the schematic%s\n",
                        core.version(), trace_enabled and " (tracing bus cycles)" or ""))
end

-- The clock does the work through its change handler. Proteus still activates
-- the model for other reasons, and there is nothing to do on those.
function device_simulate()
end
