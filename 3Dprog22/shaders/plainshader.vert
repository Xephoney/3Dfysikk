#version 410 core

layout(location = 0) in vec4 positionIn;    // 1st attribute buffer = vertex positions
layout(location = 1) in vec3 normalIn;
layout(location = 2) in vec2 uvIn;
layout(location = 3) in vec4 colorIn;       // 2nd attribute buffer = colors


out vec4 color;                             //color sent to rest of pipeline                       //the matrix for the model

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 normalTransposed = mat3(transpose(inverse(model))) * normalIn;
    //color = vec4(normalTransposed,1);
    color = colorIn;
    gl_Position = projection * view * model * positionIn;       //calculate the position of the model
}
