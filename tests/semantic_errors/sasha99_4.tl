-- test case 99.
-- Description : repeat_until

-- Expected : '4'
require "ifj21" 
function to_be_a_bee_but_bi_bee_and_maybe_be_a_bee()
    write("I'm about 2bee printed only once!")    
end                                                 
                                                    
function yours()                                    
   local a : string = "arst"                      
   repeat                                           
       to_be_a_bee_but_bi_bee_and_maybe_be_a_bee()  
       a = false                                    
   until a                                          
end                                                 
yours()                                             

