[Vertex_Shader]
// Per-vertex attributes
attribute vec4 Vertex;
attribute vec3 Normal;
attribute vec2 TexCoord;
attribute vec3 Tangent;

// Texture coordinates for the fragment shader
varying vec2 FragTexCoord;

void main()
{
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * Vertex;
  FragTexCoord = TexCoord;
  //Vertex;
  //FragTexCoord = vec2(1.0, -1.0) * TexCoord;
}

[Fragment_Shader]
// Texture unit to use for texture source
uniform sampler2D Tex0;

// Texture coordinates
varying vec2 FragTexCoord;

void main()
{
  vec4 color = texture2D(Tex0, FragTexCoord);
  gl_FragColor = color;
  //gl_FragColor = vec4(1.0, 0.0, 0.0, 0.0);
}
