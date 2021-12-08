-- test case 62.
-- Description : for cycles, type mismatched

-- Expected : '4'
require "ifj21" 
function main(iterations : number )      
    for i=0,iterations,1 do              
        for j=0,i<iterations,1 do        
            write("hello, 9 times")    
        end                              
    end                                  
end                                      
main(3)                                  

