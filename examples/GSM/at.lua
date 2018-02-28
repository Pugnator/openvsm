local at = {}

OK = "OK"
ERROR = "ERROR"

-- AT processors
function at:defproc()
	return "Not implemented yet"
end

function at:atproc()
end

-- create easy accesible AT tables
function at:init()
	for i, v in ipairs(atcmds) do
		atcmds[v[1]] = v
		atcmds[i] = nil
	end
end

function at:extractAT(str)
	local full = str:match('^%w+[+]%w+')
	if full == nil then return end
	local short = full:match('[+](.*)')
	return short:upper()
end

function at:process(str)
	print("Command to process: "..str)
	local cmd = at:extractAT(str)
	if cmd == nil then return "" end
	print("Extracted AT command: "..cmd)
	return atcmds[cmd][4]()
end

atcmds = 
{
	{"AT", OK, ERROR, atproc},
	{"IPR", OK, ERROR, at.defproc},
	{"NETSCAN", OK, ERROR, at.defproc},
	{"GMI", OK, ERROR, at.defproc},
	{"GMM", OK, ERROR, at.defproc},
	{"GMN", OK, ERROR, at.defproc},
	{"GMP", OK, ERROR, at.defproc},
	{"GMR", OK, ERROR, at.defproc},
	{"GMS", OK, ERROR, at.defproc},
	{"GSN", OK, ERROR, at.defproc},
	{"SCS", OK, ERROR, at.defproc},
	{"IMI", OK, ERROR, at.defproc},
	{"CLK", OK, ERROR, at.defproc},
	{"NUM", OK, ERROR, at.defproc},
	{"REG", OK, ERROR, at.defproc},
	{"OPS", OK, ERROR, at.defproc},
	{"POL", OK, ERROR, at.defproc},
	{"PLS", OK, ERROR, at.defproc},
	{"LCK", OK, ERROR, at.defproc},
	{"PWD", OK, ERROR, at.defproc},
	{"CUG", OK, ERROR, at.defproc},
	{"LCC", OK, ERROR, at.defproc},
	{"USD", OK, ERROR, at.defproc},
	{"PAS", OK, ERROR, at.defproc},
	{"FUN", OK, ERROR, at.defproc},
	{"PIN", OK, ERROR, at.defproc},
	{"BC", OK, ERROR, at.defproc},
	{"IND", OK, ERROR, at.defproc},
	{"PBS", OK, ERROR, at.defproc},
	{"PBR", OK, ERROR, at.defproc},
	{"PBW", OK, ERROR, at.defproc},
	{"RSM", OK, ERROR, at.defproc},
	{"MAR", OK, ERROR, at.defproc},
	{"MEE", OK, ERROR, at.defproc},
	{"SMS", OK, ERROR, at.defproc},
	{"PMS", OK, ERROR, at.defproc},
	{"LIP", OK, ERROR, at.defproc},
	{"MGF", OK, ERROR, at.defproc},
	{"SCA", OK, ERROR, at.defproc},
	{"SMP", OK, ERROR, at.defproc},
	{"SDH", OK, ERROR, at.defproc},
	{"SCB", OK, ERROR, at.defproc},
	{"SAS", OK, ERROR, at.defproc},
	{"RES", OK, ERROR, at.defproc},
	{"NMI", OK, ERROR, at.defproc},
	{"MGL", OK, ERROR, at.defproc},
	{"MGR", OK, ERROR, at.defproc},
	{"MGS", OK, ERROR, at.defproc},
	{"MSS", OK, ERROR, at.defproc},
	{"MGW", OK, ERROR, at.defproc},
	{"MGD", OK, ERROR, at.defproc},
	{"ENG", OK, ERROR, at.defproc},
	{"GDCONT", OK, ERROR, at.defproc},
	{"GDSCONT", OK, ERROR, at.defproc},
	{"GEQREQ", OK, ERROR, at.defproc},
	{"GQREQ ", OK, ERROR, at.defproc},
	{"GQMIN ", OK, ERROR, at.defproc},
	{"GEQMIN", OK, ERROR, at.defproc},
	{"GEQNEG", OK, ERROR, at.defproc},
	{"GATT", OK, ERROR, at.defproc},
	{"GACT", OK, ERROR, at.defproc},
	{"GCMOD", OK, ERROR, at.defproc},
	{"GTFT", OK, ERROR, at.defproc},
	{"GDATA", OK, ERROR, at.defproc},
	{"GPADDR", OK, ERROR, at.defproc},
	{"GCLASS", OK, ERROR, at.defproc},
	{"GREG", OK, ERROR, at.defproc},
	{"GSMS", OK, ERROR, at.defproc},
	{"POWD", OK, ERROR, at.defproc},
	{"GSMBUSY", OK, ERROR, at.defproc},
	{"RFSTS", OK, ERROR, at.defproc},
	{"VERSION", OK, ERROR, at.defproc},
	{"BAUD", OK, ERROR, at.defproc},
	{"NAME", OK, ERROR, at.defproc},
	{"PIN", OK, ERROR, at.defproc}
}

return at