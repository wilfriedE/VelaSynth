#version 320 es

precision mediump float;
in vec2 Texcoord;
out vec4 outColor;
uniform sampler2D texFramebuffer;
void main()
{
  outColor = texture(texFramebuffer, Texcoord);
float avg = 0.2126 * outColor.r + 0.7152 * outColor.g + 0.0722 * outColor.b;
outColor = vec4(avg, avg, avg, 1.0);
}