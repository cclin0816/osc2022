# OSC2022

## **BSL Library**

header only for constexpr and template support  
higher compile time, but don't seem to be resolvable now  
wait for modules (?  

## **Standard Library Usage**

some standard library function won't produce external symbol  
especially constexpr template stuff (things are in header)  
most of them will produce symbol  
just check required symbol and make sure they exsit when linking  
__always check symbol if using stdlib__  

## **Using Template**

beware using template it can cause bloated code
