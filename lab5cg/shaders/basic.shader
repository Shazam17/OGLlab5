#shader vertex
#version 330 core

layout(location = 0) in vec2 pos;

uniform mat4 u_proj;
uniform mat4 u_model;

void main() {
	gl_Position = u_proj* u_model*vec4(pos, 0, 1);
}


#shader fragment
#version 330 core
out vec4 color;


void main() {
	color = vec4(1);
}
