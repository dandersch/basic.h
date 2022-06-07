#ifdef ENABLE_WARNINGS
  WARNING_TO_ERROR("-Wshadow", 6246) // NOTE: MSVC needs /analyze flag
  WARNING_TO_ERROR("-Wshadow", 6244) // NOTE: MSVC needs /analyze flag
  // ...

  //WARNING_TO_IGNORE("-Wunknown-pragmas", 4068)

  /*
     TODO find out if there is a warning for following code:

         float f = (3/4); // == 0 when expecting 0.75

   */
#endif
