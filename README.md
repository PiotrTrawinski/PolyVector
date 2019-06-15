# PolyVector

Set of ```std::vector```-like containers optimized for use with polymorphic classes.

The usual way to hold polymorphic classes in std::vector is to use ```std::vector<Base*>``` or ```std::vector<std::unique_ptr<Base*>>```.
There are 3 major problems with this approach:
- Individual allocation and deallocation for each object (and on-heap alloc/dealloc is slow)
- Additional indirection (before accesing object's memory you need to load its pointer from memory)
- Cache unfriendliness (objects are spread out in the memory)

These problems do not occur with non-polymorphic classes, because we can just keep them in ```std::vector<Class>``` so you have:
- 1 shared allocation and deallocation for all objects (ignoring additional allocations when array grows)
- You can directly access object's memory (i-th object starts at an ```i*sizeof(Class)``` offset from the array's beginning)
- Cache friendly (all objects are allocated in a single block of memory)

We cannot use ```std::vector<Base>```, because derived classes might be bigger in size than the Base class. There are however ways to combat this problem.

For now there are 4 identified and implemented approaches:
- ```AllignedPolyVector<Base>```

  This container requires to know the maximum size of the derived class that will be stored in it. It can be calculated using provided function ```maxTypeSize<>()```.
  
  So ```MaxSize = maxTypeSize<Base, Derived1, Derived2, ..., DerivedN>()```. 
  
  Now knowing the ```MaxSize``` it just allocates ```MaxSize``` space for each object (no matter what derived class it actually is) and acts just like ```std::vector<Class>```.
  
  Note that it means you are wasting space (and so also lose cache fiendliness) when you are allocating different sized objects.
- ```PackedPolyVector<Base>```

  This approach solves the problem of wasted space wich occurs in ```AllignedPolyVector<Base>```. Instead of allocating ```MaxSize``` for each object they are just allocated one after another. 
  
  Note that because of that you cannot directly index an element of the array.
- ```IndexPackedPolyVector<Base>```

  This approach solves the inability to index elements from ```PackedPolyVector<Base>```. It does it by adding additional array which creates mapping from index to offset.
- ```MultiPolyVector<Base>```

  This creates seperate ```std::vector<>``` for each derived class.
  
  Note that with this approach you lose the ordering of objects.
