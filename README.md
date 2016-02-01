# MemoryManager

This is a simple implementation of memory manager. 
It enhances the speed of memory allocation and prevents external fragmentation.

This small piece of program is easy to use. 
It does not require any includes in your system since it overrides new directly. 

## Usage
To use the memory manager, add all the files to your project except driver.cpp.
For more configuration, check *mem_config.h* 

## Performance Reference
For 0 - 1000 bytes random size allocation without thread-safety in VS2015 release mode, 
it is about 7 times faster than malloc() and free().
   
* Check my other [portfolios](http://seanshih.com/)!
