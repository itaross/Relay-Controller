-- As a start we initialise a couple of relays. Eventually we would like to be
-- able to generate the right amount of relays programmatically using getBoard
--
-- The relay class allows the user to query the relay for state and change it's
-- state

function init_relays()
	relays = {}
	num = board.NumberOfRelays
	for i = 1,num do relays[i] = Relay:new() end
end

-- Start Class Relay
Relay = { id = 0 }

function Relay:new(o)
	o = o or {} -- create object if the user doesn't provide one
	setmetatable(o, self)
	self.__index = self
	return o
end


--function Relay:toggle()
--	board:toggleRelay(i)
--	print("toggled relay, state is now : ", self.state)
--end


function Relay:SwitchOn()
	board:commandRelay(1, 1)
	print("toggled relay to " .. state())
end

function Relay:SwitchOff()
	if self.state == 0 then print("No can do chief ! It's already off")
	else self.state = 0
	end
end

function Relay:state()
	return board:RelayState(id)
end
-- End Class Relay



