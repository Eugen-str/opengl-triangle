#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

static int winw = 500, winh = 500;

void panic_errno(const char *fmt, ...)
{
    fprintf(stderr, "ERROR: ");

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, ": %s\n", strerror(errno));

    exit(1);
}

char *slurp_file(const char *file_path)
{
#define SLURP_FILE_PANIC panic_errno("Could not read file `%s`", file_path)
    FILE *f = fopen(file_path, "r");
    if (f == NULL) SLURP_FILE_PANIC;
    if (fseek(f, 0, SEEK_END) < 0) SLURP_FILE_PANIC;

    long size = ftell(f);
    if (size < 0) SLURP_FILE_PANIC;

    char *buffer = malloc(size + 1);
    if (buffer == NULL) SLURP_FILE_PANIC;

    if (fseek(f, 0, SEEK_SET) < 0) SLURP_FILE_PANIC;

    fread(buffer, 1, size, f);
    if (ferror(f) < 0) SLURP_FILE_PANIC;

    buffer[size] = '\0';

    if (fclose(f) < 0) SLURP_FILE_PANIC;

    return buffer;
#undef SLURP_FILE_PANIC
}

void handle_shader_error(GLuint id, char *msg){
    GLint success;
    char log[1024];

    glGetShaderiv(id,GL_COMPILE_STATUS, &success);

    if(success == GL_FALSE){
        printf("Error compiling %s shader\n", msg);
        printf("Aditional info: %s", log);
    }
}

void compile_shaders(GLuint vert_shader, GLuint frag_shader, GLuint shader_program){
    const char* vert_source = slurp_file("default.vert");
    const char* frag_source = slurp_file("default.frag");

    glShaderSource(vert_shader, 1, &vert_source, NULL);
    glCompileShader(vert_shader);
    handle_shader_error(vert_shader, "vertex");

    glShaderSource(frag_shader, 1, &frag_source, NULL);
    glCompileShader(frag_shader);
    handle_shader_error(frag_shader, "fragment");

    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);

    GLint success;
    char log[1024];

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

    if(success == GL_FALSE){
        printf("Error linking program");
        printf("Aditional info: %s", log);
    }
}

int main(){
    if(!glfwInit()){
        printf("Could not initialize GLFW");
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(winw, winh, "triangle", NULL, NULL);

    if(window == NULL){
        printf("Could not create window");
        exit(1);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, winw, winh);


    GLfloat vertices [] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    GLuint indices [] = {
        0, 1, 2,
    };

    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint shader_program = glCreateProgram();
    compile_shaders(vert_shader, frag_shader, shader_program);

    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glUseProgram(shader_program);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(window)){
        glClearColor(0.071f, 0.071f, 0.071f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, sizeof(vertices) / sizeof(vertices[0]), GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
