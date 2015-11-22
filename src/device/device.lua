-- Here you can place you device code. 
-- It will be automatically compiled at build time
__USE_PRECOMPILED = 0xDEADC0DE

--NAND gate placeholder script
SAFE_MODE=true
LOGIC_TYPE=TTL

device_pins =
{
    {is_digital=true, name = "A"},
    {is_digital=true, name = "B"},
    {is_digital=true, name = "Q"},
}

function device_simulate ()
    Q:set(1-(A:get() * B:get()))
end
