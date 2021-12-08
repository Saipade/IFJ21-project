-- test case 83.
-- Description : for cycles, assignment in the assignment part.

-- Expected : '2'
require "ifj21" 
function main(iterations : number )      
    local a : integer = 10               
    for i=0, 10, a = 20 do               
    end                                  
end                                      
main("helloo")                         

