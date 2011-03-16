-- Example of a chaser using the lua command format
-- The application has an internal database of boards. If we give the name
-- of the board the app can deduce the nummber of relays and create the objects
-- necessary for the command to be run
-- The "fake board" board type at rev v0.0 has 10 relays

board.type = "fake board"
board.rev = "v0.0"
board.number_of_relays = 10

function pattern()
	for i=1,board.number_of_relays do
		if i ~= 1 then
			relays[i -1]:SwitchOff()
		end
		relays[i]:switchOn()
	end
end
