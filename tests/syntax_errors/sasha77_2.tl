-- test case 77.
-- Description : while cycle. Break cannot be without a cycle #1

-- Expected : '2'
require "ifj21" 
function main()            
   local a : integer = 10  
   if a == 10 then         
       break               
   end                     
end                        

