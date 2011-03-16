string = {}
mt = {}

function string:new()
	o = o or {} -- create object if the user doesn't provide one
	setmetatable(o, self)
	self.__index = self
	self.value = "testing"
	return o
end

function string:pprint()
	print(self.value)
end
