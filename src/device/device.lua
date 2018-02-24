-- Here you can place you device code. 
-- It will be automatically compiled at build time
-- Comment out this to use predefined script
--[[
__USE_PRECOMPILED = 0xDEADC0DE

--NAND gate placeholder script
SAFE_MODE=true
LOGIC_TYPE=TTL

device_pins =
{
    {name = "TX", on_time=100000, off_time=100000} 
}

function device_simulate ()
    Q:set(1-(A:get() * B:get()))
end
]]--