-- test case 94.
-- Description : wrong parameters

-- Expected : '5'
global name : function (integer, number, string,                   
                        string, number, integer, integer, nil, nil,
                        nil, number)                               
function name    (a : integer, b : number, c : string, d : nil) end
                                                                   
function main()                                                    
end                                                                
                                                                   
main()                                                             

