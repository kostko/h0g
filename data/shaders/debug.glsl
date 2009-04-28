[Vertex_Shader]
// Per-vertex attributes
attribute vec4 Vertex;
attribute vec3 Normal;
attribute vec2 TexCoord;
attribute vec3 Tangent;

void main()
{
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * Vertex;
}

[Fragment_Shader]
void main()
{
  // Everything is red, so it is visible
  gl_FragColor = vec4(1.0, 0.0, 0.0, 0.0);
}
