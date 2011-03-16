    Account = {balance = 0}
    
    function Account:new (o)
      o = o or {}
      setmetatable(o, self)
      self.__index = self
      return o
    end
    
    function Account:deposit (v)
      self.balance = self.balance + v
      print("deposit is : ", self.balance)
    end
    
    function Account:withdraw (v)
      if v > self.balance then error"insufficient funds" end
      self.balance = self.balance - v
    end

