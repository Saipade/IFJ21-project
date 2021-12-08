-- test case 80.
-- Description : for cycles, minus in the assignment

-- Expected : '0'
require "ifj21" 
function main(iterations : number )      
    for i=0, -iterations, -1 do          
       write("I'm gonna be printed\n")
        for j=0, i-iterations do        
            write("hello, n times")    
        end                              
    end                                  
end                                      
main(3)                                  

