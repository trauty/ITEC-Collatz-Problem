#version 330 core
layout (location = 0) in vec2 aPos;
uniform float xScale;
uniform float yScale;
uniform float xOffset;
uniform float yOffset;
void main()
{
   gl_Position = vec4((aPos.x + xOffset) * xScale, (aPos.y + yOffset) * yScale, 0, 1.0);
}