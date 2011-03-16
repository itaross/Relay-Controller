-- As a start we initialise a couple of relays. Eventually we would like to be
-- able to generate the right amount of relays programmatically using getBoard
-- 
-- The relay class allows the user to query the relay for state and change it's 
-- state

function init()
	print("Chief, I got a question. How many fu***ng relays have we got ?")
	x = io.read()
	relays = {}
	
	for i = 1,x do relays[i] = Relay:new() end
end

-- Start Class Relay
Relay = {state = 0}

function Relay:new(o)
	o = o or {} -- create object if the user doesn't provide one
	setmetatable(o, self)
	self.__index = self
	return o	
end

function Relay:toggle()
	self.state = self.state or 0
	print("toggled relay, state is now : ", self.state)
end

function Relay:SwitchOn()
	if self.state == 1 then print("No can do chief ! It's already on !")
	else self.state = 1
	end
end

function Relay:SwitchOff()
	if self.state == 0 then print("No can do chief ! It's already off")
	else self.state = 0
	end
end
-- End Class Relay



