--example counter script

SAFE_MODE = true
LOGIC_TYPE = TTL

device_pins = 
{
    {name = "A0", on_time=100000, off_time=100000},
    {name = "A1", on_time=100000, off_time=100000},   
    {name = "A2", on_time=100000, off_time=100000},   
    {name = "A3", on_time=100000, off_time=100000},   
    {name = "A4", on_time=100000, off_time=100000},   
    {name = "A5", on_time=100000, off_time=100000},   
    {name = "A6", on_time=100000, off_time=100000},   
    {name = "A7", on_time=100000, off_time=100000},   
    {name = "A8", on_time=100000, off_time=100000},   
    {name = "A9", on_time=100000, off_time=100000},   
    {name = "A10", on_time=100000, off_time=100000},   
    {name = "A11", on_time=100000, off_time=100000},   
    {name = "A12", on_time=100000, off_time=100000},   
    {name = "A13", on_time=100000, off_time=100000},   
    {name = "A14", on_time=100000, off_time=100000},   
    {name = "A15", on_time=100000, off_time=100000},   
}

PC_EVENT = 10
COUNTER = 0

function device_init()      
   set_callback(0, PC_EVENT)   
   ABUS = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15}
end

function timer_callback( time, eventid)    
    if eventid == PC_EVENT then                
		set_bus(ABUS, COUNTER)		
        COUNTER = COUNTER + 1
        set_callback(time + SEC, PC_EVENT)   
    end
end
