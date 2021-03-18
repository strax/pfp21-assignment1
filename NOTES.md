# 18.3.2021

I started the assignment by thinking about what would be the most efficient way to read the input files.
My first thought was to use streams as to keep memory usage low, as opposed to building a std::vector from the binary data.
This actually worked with std::ifstream and .read() to a uint64_t pointer (cast to *char). This worked but I read somewhere
that it does not buffer and that the API is bad. I then stumbled across C++ iterators and tried to consume the file stream as
an iterator (first with std::istream_iterator, but it is supposedly intended for text data and then with std::istreambuf_iterator)
but it did not work as the type parameter had to be `char`. I then got frustrated and decided that I could just mmap the file.

I first started implementing memory mapping directly to the main function but then realized that since C++ has classes
with destructors, I could encapsulate it into a class. This could have some advantages:
1) I could `munmap` the pages in the class destructor
2) I could use a smart pointer to manage the mapped memory lifetime
I got a maybe a little excited and went straight to generics with the value type.

NOTE: I spent a hilarious amount of time fighting linker errors because of using templates in `MemoryMappedFile`.
Turns out you just have to put everything in the header file and NOT have a `.cpp` file for the class.

# 19.3.2021

Tried to use `std::range` and `std::output_iterator` for the `MemoryMappedFile` class. Didn't work out so I just
overloaded the subscript operator. First working version.