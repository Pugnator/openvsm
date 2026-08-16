SAFE_MODE=true
LOGIC_TYPE=TTL
device_pins =
{
    {name = "A", on_time=100000, off_time=100000}, 
	{name = "B", on_time=100000, off_time=100000}, 
	{name = "Q", on_time=100000, off_time=100000} 
}
function device_simulate ()
    Q:set(1-(A:get() * B:get()))
end