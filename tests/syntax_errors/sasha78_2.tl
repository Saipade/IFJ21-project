-- test case 78.
-- Description : while cycle. Break cannot be without a cycle #2

-- Expected : '2'
require "ifj21" 
function main()            
   local a : integer = 10  
   if a ~= 10 then         
   elseif a == 10 then     
       break               
   end                     
end                        

