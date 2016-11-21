/*
 * Theoria - A C++ Application Container for the Real and Virtual World
 *
 * Copyright (c) 2016 theoriacpp.com
 * 
 * This library is released under the Apache License v. 2.0. See LICENSE in top level directory of this project.
 * or https://github.com/smangano/theoria/blob/master/LICENSE 
 */

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
      #ifdef __GNUC__
        #define DLL_PUBLIC __attribute__ ((dllexport))
        #else
          #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
          #endif
        #else
        #ifdef __GNUC__
          #define DLL_PUBLIC __attribute__ ((dllimport))
          #else
            #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
        #endif
      #endif
    #define DLL_LOCAL
#else
    #if __GNUC__ >= 4
        #define DLL_PUBLIC __attribute__ ((visibility ("default")))
      #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
    #else
        #define DLL_PUBLIC
        #define DLL_LOCAL
  #endif
#endif
