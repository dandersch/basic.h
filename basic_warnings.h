#ifdef ENABLE_WARNINGS
  /* error when shadowing variables */
  WARNING_TO_ERROR("-Wshadow", 6246) // NOTE: MSVC needs /analyze flag
  WARNING_TO_ERROR("-Wshadow", 6244) // NOTE: MSVC needs /analyze flag

  /* error when doing sizeof(pointer)/sizeof(pointer[0]) */
  WARNING_TO_ERROR("-Wsizeof-pointer-div", 6384) // NOTE: MSVC needs /analyze flag

  /* warning for: float f = (3/4); // == 0 when expecting 0.75 */
  /* NOTE: This also warns about a whole lot of other (reasonable) conversions */
  //WARNING_TO_ENABLE("-Wconversion", Unknown) // Warn for implicit conversions that may alter a value.

  /* Other warnings to consider: */
  //WARNING_TO_IGNORE("-Wunknown-pragmas", 4068)
#endif
