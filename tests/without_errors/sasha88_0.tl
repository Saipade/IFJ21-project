-- test case 88.
-- Description : break inside repuntil

-- Expected : '0'
require "ifj21" 

function me() :string                                            
    return "the whole project was written by a cat"            
end                                                              
function main()                                               
   local you : string = "atata"                             
   local truth : boolean  = true                              
   repeat                                                     
       repeat                                                 
           repeat                                             
               repeat                                         
                   repeat                                     
                       repeat                                 
                           local not_true : string = me()     
                           break                              
                           write(not_true)                    
                       until truth                            
                       break                                  
                   until truth                                
                   break                                      
               until truth                                    
               break                                          
           until truth                                        
           break                                              
       until truth                                            
       break                                                  
   until truth                                                
                                                              
   return                                                     
end                                                           
main()                                                        

