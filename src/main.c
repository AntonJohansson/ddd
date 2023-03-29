#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include "sokol_gfx.h"
#include "sokol_log.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "mat.h"
#include "quat.h"
#include "math.h"

typedef struct Uniforms {
    Mat4 mvp;
} Uniforms;

static inline Camera create_camera(Vec3 pos, Vec3 dir) {
    const Vec3 world_up = {0,1,0};

    const Vec3 i = vec3_cross(dir, world_up);
    const Vec3 j = vec3_cross(i, dir);
    const Vec3 k = vec3_neg(dir);

    return (Camera) {
        .pos = pos,
        .i = i,
        .j = j,
        .k = k,
    };
}

int main() {
    /* create window and GL context via GLFW */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(640, 480, "floating", 0, 0);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* setup sokol_gfx */
    sg_setup(&(sg_desc){
        .logger.func = slog_func,
    });

    /* a vertex buffer */
    const float vertices[] = {
        // positions            // colors
         0.0f,  0.5f, 0.0f,     1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f, 1.0f
    };
    sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
        .data = SG_RANGE(vertices)
    });

    /* a shader */
    sg_shader shd = sg_make_shader(&(sg_shader_desc){
        .vs.uniform_blocks = {
            [0] = {
                .size = sizeof(Uniforms),
                .uniforms = {
                    [0] = { .name = "mvp", .type = SG_UNIFORMTYPE_MAT4 },
                },
            },
        },
        .vs.source =
            "#version 330\n"
            "layout(location=0) in vec3 position;\n"
            "layout(location=1) in vec4 color0;\n"
            "uniform mat4 mvp;\n"
            "out vec4 color;\n"
            "void main() {\n"
            "  gl_Position = mvp * vec4(position, 1);\n"
            "  color = color0;\n"
            "}\n",
        .fs.source =
            "#version 330\n"
            "in vec4 color;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "  frag_color = color;\n"
            "}\n"
    });

    /* a pipeline state object (default render states are fine for triangle) */
    sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .layout = {
            .attrs = {
                [0].format=SG_VERTEXFORMAT_FLOAT3,
                [1].format=SG_VERTEXFORMAT_FLOAT4
            }
        }
    });

    /* resource bindings */
    sg_bindings bind = {
        .vertex_buffers[0] = vbuf
    };

    /* default pass action (clear to grey) */
    sg_pass_action pass_action = {0};

    double prev_mouse_x = 0;
    double prev_mouse_y = 0;

    double yaw = -M_PI/2.0f;
    double pitch = 0.0f;

    Vec3 pos = {0};

    double t = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        double start = glfwGetTime();

        double mouse_x;
        double mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        double dx = mouse_x - prev_mouse_x;
        double dy = mouse_y - prev_mouse_y;

        yaw += 0.001f*dx;
        pitch += 0.001f*dy;
        pitch = CLAMP(-M_PI/2.0f+0.1f, pitch, M_PI/2.0f-0.1f);

        Vec3 dir = (Vec3){cosf(yaw)*cosf(pitch), -sinf(pitch), sinf(yaw)*cosf(pitch)};
        Camera camera = create_camera(pos, dir);

        Vec3 move = {0};
        if (glfwGetKey(window, GLFW_KEY_A) != GLFW_RELEASE) move = vec3_add(move, vec3_scale(-1.0f, camera.i));
        if (glfwGetKey(window, GLFW_KEY_D) != GLFW_RELEASE) move = vec3_add(move, vec3_scale(+1.0f, camera.i));
        if (glfwGetKey(window, GLFW_KEY_W) != GLFW_RELEASE) move = vec3_add(move, vec3_scale(-1.0f, camera.k));
        if (glfwGetKey(window, GLFW_KEY_S) != GLFW_RELEASE) move = vec3_add(move, vec3_scale(+1.0f, camera.k));

        if (!float_equal(vec3_len2(move), 0.0f)) {
            camera.pos = vec3_add(camera.pos, vec3_scale(0.025f, vec3_normalize(move)));
            pos = camera.pos;
        }

        // model:       model space -> world space
        // view:        world space -> view space
        // projection:  view space  -> homogenous clip space
        Mat4 model = mat4_model((Vec3){0.0f, 0.0f, 5.0f});
        Mat4 view = mat4_view_from_camera(camera);
        Mat4 projection = mat4_projection(0.1f, 10.0f, 16.0f/9.0f, 40.0f);
        Uniforms uniforms = {
            .mvp = mat4_transpose(mat4_mul(projection, mat4_mul(view, model))),
        };

        int cur_width, cur_height;
        glfwGetFramebufferSize(window, &cur_width, &cur_height);
        sg_begin_default_pass(&pass_action, cur_width, cur_height);
        sg_apply_pipeline(pip);
        sg_apply_bindings(&bind);
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &SG_RANGE(uniforms));
        sg_draw(0, 3, 1);
        sg_end_pass();
        sg_commit();
        glfwSwapBuffers(window);
        glfwPollEvents();

        prev_mouse_x = mouse_x;
        prev_mouse_y = mouse_y;

        double end = glfwGetTime();
        t += end-start;
    }

    glfwSetInputMode(window, GLFW_CURSOR, ~GLFW_CURSOR_DISABLED);

    /* cleanup */
    sg_shutdown();
    glfwTerminate();
    return 0;
}
