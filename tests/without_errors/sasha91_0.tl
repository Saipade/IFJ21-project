-- test case 91.
-- Description : no errors, braced expressions

-- Expected : '0'
require "ifj21" 
function write_numbers(counter : integer)
   while (counter) > 0 do                
        write(counter, "
")           
    counter = counter - 1                
    end                                  
end                                      

