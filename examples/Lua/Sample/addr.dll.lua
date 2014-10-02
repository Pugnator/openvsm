device_pins = 
{
    {is_digital=true, name = "A0", on_time=100000, off_time=100000},
    {is_digital=true, name = "A1", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A2", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A3", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A4", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A5", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A6", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A7", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A8", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A9", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A10", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A11", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A12", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A13", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A14", on_time=100000, off_time=100000},   
    {is_digital=true, name = "A15", on_time=100000, off_time=100000},   
}

PC_EVENT = 0
COUNTER = 0

function device_init()   
   for k, v in pairs(device_pins) do set_pin_state(_G[v.name], SLO) end     
   set_callback(0, PC_EVENT)   
end

function timer_callback(time, eventid)    
    if eventid == PC_EVENT then        
        for k, v in pairs(device_pins) do 
            set_pin_bool(_G[v.name], get_bit(COUNTER, k) )           
        end
        COUNTER = COUNTER + 1
        set_callback(time + 100 * MSEC, PC_EVENT)   
    end
end
