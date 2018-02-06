# Math Expression Recognizer
Math Expression Recognizer based on SLR Parser.  
Scanner based on NFA (Nondeterministic finite automaton)

## Input
File `expression.txt` containig math expression.  
For example the next expression will be recognized successfully:  
`(-ln(e)) + 4 * x * (7) / x^5 + sin(x + 8)`  
`sinlg(x * (254 + 80.55)) / ln(e)`  

## Output
The next files will be output:  
`protocol.txt` - description of every operation with result  
`tokens.txt` - if Scanner hasn't found any lexical errors this file containt list of recognized tokens  
`LR(0).txt` - set of LR(0)-points produced by Parser  
`slr-table.txt` - SLR Table produced by Parser
