-- test case 97.
-- Description : condition with wrong types. Condition can be nil

-- Expected : '0'
require "ifj21" 
function write_numbers(counter : nil)    
   while counter do                     
        write(counter, "
")           
    end                                  
end                                      

