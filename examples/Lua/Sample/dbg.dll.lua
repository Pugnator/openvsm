-- Device description
device_pins =
{
    {is_digital=true, name = "D0", on_time=1000, off_time=1000},
    {is_digital=true, name = "D1", on_time=1000, off_time=1000},
    {is_digital=true, name = "D2", on_time=1000, off_time=1000},
    {is_digital=true, name = "D3", on_time=1000, off_time=1000},
    {is_digital=true, name = "D4", on_time=1000, off_time=1000},
    {is_digital=true, name = "D5", on_time=1000, off_time=1000},
    {is_digital=true, name = "D6", on_time=1000, off_time=1000},
    {is_digital=true, name = "D7", on_time=1000, off_time=1000},
    {is_digital=true, name = "TX", on_time=1000, off_time=1000},
}
-- UART events
UART_STOP = 0
UART_START = 1
UART_DATA=2
-- Constants
BAUD=9600
BAUDCLK = SEC/BAUD
BIT_COUNTER = 0
-----------------------------------------------------------------
DATA_BUS = 0

function device_init()
    -- console_alloc("sdfsdsdfsfsf")
end

function device_simulate()
    for i = 0, 7 do        
        if 1 == get_pin_bool(_G["D"..i]) then            
            DATA_BUS = set_bit(DATA_BUS, i)
        else            
            DATA_BUS = clear_bit(DATA_BUS, i)
        end
    end 
    uart_send(string.format("[%d] Fetched opcode %.2X\r\n", systime(), DATA_BUS))  
end

function timer_callback(time, eventid)
    uart_callback(time, eventid)
end

function uart_send (string)
    uart_text = string
    char_count = 1
    set_pin_state(TX, SHI) -- set TX to 1 in order to have edge transition
    set_callback(BAUDCLK, UART_START) --schedule start
end

function uart_callback (time, event)
    if event == UART_START then
        next_char = string.byte(uart_text, char_count)

        if next_char == nil then
            return
        end
        char_count = char_count +1
        set_pin_state(TX, SLO)
        set_callback(time + BAUDCLK, UART_DATA)
    end

    if event == UART_STOP then
        set_pin_state(TX, SHI)
        set_callback(time + BAUDCLK, UART_START)
    end

    if event == UART_DATA then

        if get_bit(next_char, BIT_COUNTER) == 1 then
            set_pin_state(TX, SHI)
        else
            set_pin_state(TX, SLO)
        end
        if BIT_COUNTER == 7 then
            BIT_COUNTER = 0
            set_callback(time + BAUDCLK, UART_STOP)
            return
        end
        BIT_COUNTER = BIT_COUNTER + 1
        set_callback(time + BAUDCLK, UART_DATA)
    end
end
