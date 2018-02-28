cmd = require "at"
cmd.init()
while(true) do
	input = io.read()
	print(cmd:process(input))
end