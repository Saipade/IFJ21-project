-- test case 98.
-- Description : condition with wrong syntax. nil cannot have length

-- Expected : '2'
require "ifj21" 
function write_numbers(counter : nil)    
   while #counter do                     
        write(counter, "
")           
    end                                  
end                                      

