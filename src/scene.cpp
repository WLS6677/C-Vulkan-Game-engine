#include <scene.h>

// will convert all chunks and objects to meshes/WLRenderObjects and send them to the GPU
// TODO: make a ver that only submits changed chunks
void wlCreateSceneMeshObjects(WLScene scene){

    uint32_t render_object_count = 0;
    render_object_count += scene.chunk_count;
    //TODO: add actuall entities and assets

    WLRenderObject pRender_objects[render_object_count];

    for (size_t i = 0; i < scene.chunk_count; i++){
        pRender_objects[i] = WLGetChunkSVOMesh(scene.pChunks[i]);
    }
    
    wlInitVertexBuffer(pRender_objects, render_object_count);

    //TODO: free the WL render objects from the allocator
}

glm::mat4x4 wlGetCameraMatrix(WLScene scene){
    // controlling the camera is as simple as translating it and moving the yaw and pitch with mouse

    // getting the axis that the camera is looking in the direction of
    vec3f forward_direction_vector; // Z axis in camera space
    forward_direction_vector.x = cos(scene.camera.pitch) * cos(scene.camera.yaw);
    forward_direction_vector.y = sin(scene.camera.pitch);
    forward_direction_vector.z = cos(scene.camera.pitch) * sin(scene.camera.yaw);

    vec3f right_direction_vector; // X axis in camera space
    // the X product of the world vertical axis and the forward would give the right vector
    right_direction_vector = wlget32fVec3Normalized(wlCross32fVec3(forward_direction_vector, vec3f{0.0f, 0.0f, 1.0f}));

    vec3f up_direction_vector; // Y axis in camera space
    up_direction_vector = wlget32fVec3Normalized(wlCross32fVec3(forward_direction_vector, right_direction_vector));

    glm::mat4x4 view_matrix = glm::lookAt(
        glm::vec3 {scene.camera.position.x,scene.camera.position.y,scene.camera.position.z},
        glm::vec3 {forward_direction_vector.x + scene.camera.position.x,forward_direction_vector.y + scene.camera.position.y,forward_direction_vector.z + scene.camera.position.z},
        glm::vec3 {up_direction_vector.x,up_direction_vector.y,up_direction_vector.z}
    );

    // TODO: make the aspect ration and FOV dynamic and editable in settings
    glm::mat4 projection_matrix = glm::perspective(glm::radians(90.0f), (float) 16 / (float)9, 0.1f, 100.0f);
    // Flip Y for Vulkan
    projection_matrix[1][1] *= -1;

    return projection_matrix * view_matrix;
}
