-- test case 86.
-- Description : for cycles, no error.

-- Expected : '0'
require "ifj21" 
function main(iterations : number )      
    local a : integer = 10               
    for a=0, 20, 2 do                    
       break                             
    end                                  
end                                      
main("helloo")                         

