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











example code for a train mod C#




////////////////////////////////////
          engine API
////////////////////////////////////

LOD_ChunkObject



////////////////////////////////////////
            mode code           
//////////////////////////////////////

class railNode{
    vec3 position;
    vec3 railDirectionVector;
    int rail_type;
    
    int uniquehash; //so nodes can refernce each other

    float distanceToNextNode;
    float distanceTopreviousNode;
}

class rail_path : LOD_ChunkObject {

    //LOD_ChunkObject position
    vec3 position

    List<railNode> nodes;
    // i can convert the data from the nodes into a rail track
    float trackLength;
    int uniqueTrackHash;
    nextTrackHash;
    previousTrackHash;

    mat4 TrackPosToWorldPos(float DistanceFromStart){
        if(DIstance > trackLenght){
            //use the next track or derail
        }
        if(Distance < 0){
            // use the previous track or derail
        }
    }
}

List<railNod> findRailsInChunk(){
    // loops on all the nodes and forms sorts them into rail
}



class train : LOD_ChunkObject {
    bool isRailed;

}

