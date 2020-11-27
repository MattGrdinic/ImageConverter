# ImageConverter
Convert RGB Images To CMYK

This project showcases various optimization techniques for the programatic process of converting an RGB source image into CMYK color space.

We start with the Qt frameworks fp.convertTo() function, then work our way up though 9 different and progressively complex techniques. Along the way we experiment with loop ordering, cache coherence, other standard optimization techniques.  

As this project is written in C++, our optimization samples start with basic nested foreach loops using type casts, and end up with hand-crafted SIMD Floating-Point Intrinsics written in assembly.

The standard lesson of this exercise is code optimization can become a zero-sum game in terms of performance gain vs. code complexity and maintainability. This is especially evident in the final optimizations!
