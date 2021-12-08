-- test case 93.
-- Description : no errors, comments

-- Expected : '0'
require "ifj21"                                                                            
                                                                                             
function write_numbers(counter : integer)                                                    
    while (counter) > 0 do                                                                   
        write(counter, "
")                                                               
        counter = counter - 1                                                                
    end                                                                                      
end                                                                                          
function main()                                                                              
    local counter : integer = readi()                                                        
                                                                                             
    if counter == 0 then                                                                     
        write("Error", " enter",                                                         
              " another", " number",                                                     
              ", because ", 0, " is wrong","
")                                       
        return                                                                               
    else                                                                                     
        write_numbers(counter)                                                               
    end                                                                                      
                                                                                             
end                                                                                          
                                                                                             
--[[                                                                                         
function main()                                                                              
    local counter : integer = readi()                                                        
                                                                                             
    if counter == 0 then                                                                     
        write("Error", " enter",                                                         
              " another", " number",                                                     
              ", because ", 0, " is wrong","
")                                       
        return                                                                               
    else                                                                                     
        write_numbers(counter)                                                               
    end                                                                                      
                                                                                             
end                                                                                          
]]                                                                                           
                                                                                             
-- main()                                                                                    
                                                                                             

