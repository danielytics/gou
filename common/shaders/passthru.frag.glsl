// #version 450 core

// out vec4 FragColor;
  
// in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

// void main()
// {
//     FragColor = vertexColor;
// } 

#version 450 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
} 