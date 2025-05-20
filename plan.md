naming convention for the API code is:
variables: PascalCase
functions: camelCase
function arguements: _idk_what_this_case_is 
defines/constexpr/enums: SCREAMING_SNAKE_CASE
internal variables: normal_snake_case          *inside structs/functions*
       

we will have modules that will make up the entire programm, the modules consist of:
a header to define the API
the SRC implemntation

the modules will be used as API in the main core module
the API needs these:
  a create_function that returns a pointer to the module instance
  all functions expect the module instance to run
  a destroy_function that frees all the data

the module instance will be where all of the module data 


https://vkguide.dev/docs/new_chapter_4/engine_arch/