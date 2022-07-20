#pragma once
#include <filesystem>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "structs.hxx"
#include "animation.hxx"
#include "numbers.hxx"
#include "lodepng.h"
#include "myimgui.h"


#include "glad/glad.h"

#define map_start_X 0.5
#define map_start_Y 0.5
#define map_scale

static void APIENTRY callback_opengl_debug(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, [[maybe_unused]] const void* userParam);



class engine{
    //openGL programs
    GLuint        program_id_background = 0;
    GLuint        program_id_body = 0;


    //textures
    GLuint tex_handl [20];
    //----SDL
    SDL_Window *window=NULL;
    SDL_Event event_log;
    SDL_Renderer *rend=NULL;
    SDL_GLContext gl_context=NULL;

    //--MAP
    triangle background1;
    triangle background2;

    triangle body1;
    triangle body2;
    //runners

    //text
    TTF_Font *gFont = NULL;
    std::vector<SDL_Texture*> names;

    //numbers for timer
    numbersText* numbers_=NULL;


    glm::mat4 buffMat=glm::mat4(1.0f);


    //timer
    bool boolTime=true;    
    Uint32 startTime = 0;
    int buffTimerTime=180;
    uint32_t timerTime = 0;
    Uint32 currentTime = 0;
    Uint32 bufferTime = 0;
    Uint32 diffTime = 0;
    Uint32 lastTime=110;
    uint16_t minutes_=0;
    uint16_t seconds_=0;

    //water moving
    int angle_old=0;
    int angle_new=0;
    glm::mat4 buffSea=glm::mat4(1.0f);
    glm::mat4 rotateMat=glm::mat4(1.0f);
    float water_power=0.001;
    float moveX=0.0f;
    float moveY=0.0f;


    //
    triangle rock;
    bool crash=false;


public:
    engine(){

                if( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    SDL_Quit();;
                }

                std::stringstream serr;
                using namespace std;

                const int init_result = SDL_Init(SDL_INIT_TIMER
                                                 |SDL_INIT_VIDEO
                                                 |SDL_INIT_EVENTS );
                if (init_result != 0)
                {
                    const char* err_message = SDL_GetError();
                     serr << "error: failed call SDL_Init: " << err_message << std::endl;
                    //return serr.str();
                }





                SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_RESET_ISOLATION_FLAG);

                window=SDL_CreateWindow("runners",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          900,
                                          900,
                                          SDL_WINDOW_OPENGL
                                          );

                   if(window==NULL){
                   std::cout<<"Couldn't create window"<<SDL_GetError()<<std::endl;
                       SDL_DestroyWindow(window);
                   }

                   rend=SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED
                                           | SDL_RENDERER_PRESENTVSYNC
                                           | SDL_RENDERER_TARGETTEXTURE);
                   if(rend==NULL){
                        const char* err_message = SDL_GetError();
                         serr << "error: failed call createRenderer: " << err_message << std::endl;
                   }

                   int gl_major_ver       = 3;//3
                   int gl_minor_ver       = 0;//2
                   int gl_context_profile = SDL_GL_CONTEXT_PROFILE_ES;

                   const char* platform_from_sdl = SDL_GetPlatform();
                   std::string_view platform{platform_from_sdl};
                   using namespace std::string_view_literals;
                   using namespace std;
                   auto list = { "Windows"sv, "Mac OS X"sv };
                   auto it   = find(begin(list), end(list), platform);
                   /*if (it != end(list))
                   {
                       gl_major_ver       = 4;
                       gl_minor_ver       = (platform == "Mac OS X") ? 1 : 3;
                       gl_context_profile = SDL_GL_CONTEXT_PROFILE_ES;
                   }*/

                   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_context_profile);
                   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
                   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);

                   SDL_GLContext gl_context = SDL_GL_CreateContext(window);
                   if (gl_context == nullptr)
                   {
                       gl_major_ver       = 3;//3
                       gl_minor_ver       = 0;//2
                       gl_context_profile = SDL_GL_CONTEXT_PROFILE_ES;

                       SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_context_profile);
                       SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
                       SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);
                       gl_context = SDL_GL_CreateContext(window);
                   }
                   assert(gl_context != nullptr);

                   int result =
                       SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
                   assert(result == 0);
                   result =
                       SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
                   assert(result == 0);

                   if (gl_major_ver < 3)
                   {
                       std::clog << "current context opengl version: " << gl_major_ver
                                 << '.' << gl_minor_ver << '\n'
                                 << "need openg ES version at least: 3.2\n"
                                 << std::flush;
                       throw std::runtime_error("opengl version too low");
                   }

                   std::clog << "OpenGl "<< gl_major_ver << '.'<< gl_minor_ver<<'\n';

                   if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0)
                   {
                       std::clog << "error: failed to ialize glad" << std::endl;
                   }

                   if (platform != "Mac OS X") // not supported on Mac
                   {
                       glEnable(GL_DEBUG_OUTPUT);
                       glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                       glDebugMessageCallback(callback_opengl_debug, nullptr);
                       glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                                         nullptr, GL_TRUE);
                   }

                   glGenTextures(20, tex_handl); //texture descriptor
                   OM_GL_CHECK()


                   gl_context=SDL_GL_CreateContext(window);

                   imgui_init(window,&gl_context,rend);

                   SDL_ShowCursor(SDL_ENABLE);




    }
    ~engine(){
        delete numbers_;
        imgui_shutdown();

        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        window=NULL;
        rend=NULL;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();

    }
    bool load_texture(std::string_view path,int number){
        const char* filename=&path[0];
        std::vector<unsigned char> image;
        unsigned int          w = 0;
        unsigned int          h = 0;
        int error = lodepng::decode(image,w,h,filename);
        /*GLuint tex_handl = 0;
        glGenTextures(1, &tex_handl); //texture descriptor
        OM_GL_CHECK()
        glBindTexture(GL_TEXTURE_2D, tex_handl);
        OM_GL_CHECK()*/
        glActiveTexture(GL_TEXTURE0+number);
        glBindTexture(GL_TEXTURE_2D,tex_handl[number]);
        GLint mipmap_level = 0;
        GLint border       = 0;
        glTexImage2D(GL_TEXTURE_2D, // type of texture
                     mipmap_level,  //
                     GL_RGBA,       //color format in
                     static_cast<GLsizei>(w),//texture weight
                     static_cast<GLsizei>(h),//texture height
                     border,
                     GL_RGBA,       // color format out(color what we want to get in opengl)
                     GL_UNSIGNED_BYTE, // Specifies the data type of the texel data
                     &image[0]);
        OM_GL_CHECK()
        //making correct color of zoomed texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        OM_GL_CHECK()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        OM_GL_CHECK()
        return true;

    }
    void swapBuffers(){
        SDL_GL_SwapWindow(window);
    }
    void initfTriagBackground(){

        //rock
        //1
        rock.v[0].x=0.5;
        rock.v[0].y=0.8;
        rock.v[0].tex_x=0.0;
        rock.v[0].tex_y=1.0;
        //2
        rock.v[1].x=0.8;
        rock.v[1].y=0.3;
        rock.v[1].tex_x=0.0;
        rock.v[1].tex_y=0.0;
        //3
        rock.v[2].x=0.3;
        rock.v[2].y=0.3;
        rock.v[2].tex_x=1.0;
        rock.v[2].tex_y=1.0;


        background1.v[0].x=-1;
        background1.v[0].y=-1;
        background1.v[0].tex_x=0.0;
        background1.v[0].tex_y=1.0;
        //2
        background1.v[1].x=-1;
        background1.v[1].y=1;
        background1.v[1].tex_x=0.0;
        background1.v[1].tex_y=0.0;
        //3
        background1.v[2].x=1;
        background1.v[2].y=-1;
        background1.v[2].tex_x=1.0;
        background1.v[2].tex_y=1.0;
        //
        background2.v[0].x=-1;
        background2.v[0].y=1;
        background2.v[0].tex_x=0.0;
        background2.v[0].tex_y=0.0;
        //2
        background2.v[1].x=1;
        background2.v[1].y=1;
        background2.v[1].tex_x=1.0;
        background2.v[1].tex_y=0.0;
        //3
        background2.v[2].x=1;
        background2.v[2].y=-1;
        background2.v[2].tex_x=1.0;
        background2.v[2].tex_y=1.0;


        body1.v[0].x=-0.2;
        body1.v[0].y=-0.2;
        body1.v[0].tex_x=0.0;
        body1.v[0].tex_y=1.0;
        //2
        body1.v[1].x=-0.2;
        body1.v[1].y=0.2;
        body1.v[1].tex_x=0.0;
        body1.v[1].tex_y=0.0;
        //3
        body1.v[2].x=0.2;
        body1.v[2].y=-0.2;
        body1.v[2].tex_x=1.0;
        body1.v[2].tex_y=1.0;
        //
        body2.v[0].x=-0.2;
        body2.v[0].y=0.2;
        body2.v[0].tex_x=0.0;
        body2.v[0].tex_y=0.0;
        //2
        body2.v[1].x=0.2;
        body2.v[1].y=0.2;
        body2.v[1].tex_x=1.0;
        body2.v[1].tex_y=0.0;
        //3
        body2.v[2].x=0.2;
        body2.v[2].y=-0.2;
        body2.v[2].tex_x=1.0;
        body2.v[2].tex_y=1.0;

    }
    bool event(){
        while(SDL_PollEvent(&event_log)){
            ImGui_ImplSDL2_ProcessEvent(&event_log);
            if(event_log.type==SDL_QUIT){
                return false;
            }else if(event_log.type==SDL_KEYDOWN){
                if(event_log.key.keysym.scancode==SDL_SCANCODE_SPACE){
                    buffMat=glm::translate(buffMat,glm::vec3(0.0f,0.01,0.0f));
                    return true;
                }
            }
        }

        return true;
    }
    std::string initProgramBackground(){
        std::stringstream serr;
        //vertex
        GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
        OM_GL_CHECK()
        std::string_view vertex_shader_src = R"(
                                             #version 300 es

                                             layout(location=0)in vec4 vPosition;
                                             in vec2 a_tex_coord;

                                             out vec2 v_tex_coord;

                                             void main()
                                             {
                                                 v_tex_coord=a_tex_coord;
                                                 gl_Position=vPosition;//vertex pos
                                             }
                                        )";
        const char* source            = vertex_shader_src.data();
        glShaderSource(vert_shader, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(vert_shader);
        OM_GL_CHECK()

        GLint compiled_status = 0;
        glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(vert_shader, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(vert_shader);
            OM_GL_CHECK()

            std::string shader_type_name = "vertex";
            serr << "Error compiling shader(vertex)\n"
                 << vertex_shader_src << "\n"
                 << info_chars.data();
            return serr.str();
        }
        //fragment
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        OM_GL_CHECK()
        std::string_view fragment_shader_src = R"(
                                               #version 300 es
                                               precision mediump float;

                                               in vec2 v_tex_coord;

                                               uniform sampler2D s_texture;

                                               out vec4 frag_color;
                                               void main()
                                               {
                                                 frag_color = texture(s_texture, v_tex_coord);
                                               }
                          )";
        /**/
        source                          = fragment_shader_src.data();
        glShaderSource(fragment_shader, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(fragment_shader);
        OM_GL_CHECK()

        compiled_status = 0;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(
                fragment_shader, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(fragment_shader);
            OM_GL_CHECK()

            serr << "Error compiling shader(fragment)\n"
                 << vertex_shader_src << "\n"
                 << info_chars.data();
            return serr.str();
        }
        //union shaderis in one program

        program_id_background = glCreateProgram();
        OM_GL_CHECK()
        if (0 == program_id_background)
        {
            serr << "failed to create gl program";
            return serr.str();
        }

        glAttachShader(program_id_background, vert_shader);
        OM_GL_CHECK()
        glAttachShader(program_id_background, fragment_shader);
        OM_GL_CHECK()

        // bind attribute location
        glBindAttribLocation(program_id_background, 0, "a_position");
        OM_GL_CHECK()
        // link program after binding attribute locations
        glLinkProgram(program_id_background);
        OM_GL_CHECK()
        // Check the link status
        GLint linked_status = 0;
        glGetProgramiv(program_id_background, GL_LINK_STATUS, &linked_status);
        OM_GL_CHECK()
        if (linked_status == 0)
            {
                GLint infoLen = 0;
                glGetProgramiv(program_id_background, GL_INFO_LOG_LENGTH, &infoLen);
                OM_GL_CHECK()
                    std::vector<char> infoLog(static_cast<size_t>(infoLen));
                glGetProgramInfoLog(program_id_background, infoLen, nullptr, infoLog.data());
                OM_GL_CHECK()
                serr << "Error linking program:\n" << infoLog.data();
                glDeleteProgram(program_id_background);
                OM_GL_CHECK()
                return serr.str();
            }
        return "";

    }
    std::string initProgramBody(){
        std::stringstream serr;
        //vertex
        GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
        OM_GL_CHECK()
        std::string_view vertex_shader_src = R"(
                                             #version 300 es
                                             uniform mat4 rot_matrix;
                                             layout(location=0)in vec4 vPosition;
                                             in vec2 a_tex_coord;

                                             out vec2 v_tex_coord;

                                             void main()
                                             {
                                                 v_tex_coord=a_tex_coord;
                                                 gl_Position=rot_matrix * vPosition;//vertex pos
                                             }
                                        )";
        const char* source            = vertex_shader_src.data();
        glShaderSource(vert_shader, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(vert_shader);
        OM_GL_CHECK()

        GLint compiled_status = 0;
        glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(vert_shader, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(vert_shader);
            OM_GL_CHECK()

            std::string shader_type_name = "vertex";
            serr << "Error compiling shader(vertex)\n"
                 << vertex_shader_src << "\n"
                 << info_chars.data();
            return serr.str();
        }
        //fragment
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        OM_GL_CHECK()
        std::string_view fragment_shader_src = R"(
                                               #version 300 es
                                               precision mediump float;

                                               in vec2 v_tex_coord;

                                               uniform sampler2D s_texture;

                                               out vec4 frag_color;
                                               void main()
                                               {
                                                 frag_color = texture(s_texture, v_tex_coord);
                                               }
                          )";
        /**/
        source                          = fragment_shader_src.data();
        glShaderSource(fragment_shader, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(fragment_shader);
        OM_GL_CHECK()

        compiled_status = 0;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(
                fragment_shader, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(fragment_shader);
            OM_GL_CHECK()

            serr << "Error compiling shader(fragment)\n"
                 << vertex_shader_src << "\n"
                 << info_chars.data();
            return serr.str();
        }
        //union shaderis in one program

        program_id_body = glCreateProgram();
        OM_GL_CHECK()
        if (0 == program_id_body)
        {
            serr << "failed to create gl program";
            return serr.str();
        }

        glAttachShader(program_id_body, vert_shader);
        OM_GL_CHECK()
        glAttachShader(program_id_body, fragment_shader);
        OM_GL_CHECK()

        // bind attribute location
        glBindAttribLocation(program_id_body, 0, "a_position");
        OM_GL_CHECK()
        // link program after binding attribute locations
        glLinkProgram(program_id_body);
        OM_GL_CHECK()
        // Check the link status
        GLint linked_status = 0;
        glGetProgramiv(program_id_body, GL_LINK_STATUS, &linked_status);
        OM_GL_CHECK()
        if (linked_status == 0)
            {
                GLint infoLen = 0;
                glGetProgramiv(program_id_body, GL_INFO_LOG_LENGTH, &infoLen);
                OM_GL_CHECK()
                    std::vector<char> infoLog(static_cast<size_t>(infoLen));
                glGetProgramInfoLog(program_id_body, infoLen, nullptr, infoLog.data());
                OM_GL_CHECK()
                serr << "Error linking program:\n" << infoLog.data();
                glDeleteProgram(program_id_body);
                OM_GL_CHECK()
                return serr.str();
            }

        return "";

    }
    std::string activateProgBackground(uint8_t text_num){
        glUseProgram(program_id_background);
        OM_GL_CHECK()

        int location = glGetUniformLocation(program_id_background, "s_texture");
        OM_GL_CHECK()
        assert(-1 != location);
        int texture_unit = text_num;//number of texture
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        OM_GL_CHECK()

        glUniform1i(location, 0 + texture_unit);
        OM_GL_CHECK()

        glEnable(GL_BLEND);
        OM_GL_CHECK()
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        OM_GL_CHECK()

        return "";
    }
    std::string activateProgBody(uint8_t text_num, glm::mat4 mat_in){
        // turn on rendering with just created shader program
        glUseProgram(program_id_body);
        OM_GL_CHECK()

        GLuint mem=glGetUniformLocation(program_id_body,"rot_matrix");
        OM_GL_CHECK()

        glUniformMatrix4fv(mem,1,GL_FALSE,glm::value_ptr(mat_in));
        OM_GL_CHECK()


        int location = glGetUniformLocation(program_id_body, "s_texture");
        OM_GL_CHECK()
        assert(-1 != location);
        int texture_unit = text_num;//number of texture
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        OM_GL_CHECK()

        glUniform1i(location, 0 + texture_unit);
        //OM_GL_CHECK()

        glEnable(GL_BLEND);
        //OM_GL_CHECK()
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //OM_GL_CHECK()

        return "";
    }
    void render_triangle(const triangle& t){

        //vertexs
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                              &t.v[0].x);
        //OM_GL_CHECK()

        glEnableVertexAttribArray(0);
        //OM_GL_CHECK()
        //textures
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                             &t.v[0].tex_x);
        //OM_GL_CHECK()
        glEnableVertexAttribArray(1);
        OM_GL_CHECK()

        glDrawArrays(GL_TRIANGLES, 0, 3);
        OM_GL_CHECK()

    }
    void renderOneColGL(){
        glClearColor(0.357,0.950,0.665,0.0f);
        OM_GL_CHECK()
        glClear(GL_COLOR_BUFFER_BIT);
        OM_GL_CHECK()
    }
    void render_background(int pos){
       //background
       activateProgBackground(pos);
       render_triangle(background1);
       render_triangle(background2);

    }
    void render_sprite(const sprite& t){
        render_triangle(t.triag_1);
        render_triangle(t.triag_2);
    }
    void createTextureName(std::string& text,int position){
        //std::string text="fff";
        int height=96;
       gFont=TTF_OpenFont("multimarkdown_NotoSans-Bold.ttf",height);
        if (gFont == nullptr){
            printf( "SDL_ttf bad with openFont! "
                    "SDL_ttf Error: %s\n", TTF_GetError() );
             SDL_Quit();
      }
       SDL_Surface *surf = TTF_RenderUTF8_Solid(
                    gFont,text.c_str(),{255,0,0,0}
                    );
        if (surf == nullptr){
            TTF_CloseFont(gFont);
            printf( "SDL_ttf bad with TTF_RenderText_Solid! "
                    "SDL_ttf Error: %s\n", TTF_GetError() );
        }

        int w =
                pow(2, ceil( log(surf->w)/log(2) ) );

        std::cout<<surf->w<<":"<<surf->h<<" text texture - "<<w<<std::endl;

        SDL_Surface* newSurface =
        SDL_CreateRGBSurface(0, surf->w, surf->h, 24, 0xff000000, 0x00ff0000, 0x0000ff00, 0);
        //SDL_CreateRGBSurface(0, w, w, 24, 0xff000000, 0x00ff0000, 0x0000ff00, 0);
        SDL_BlitSurface(surf, 0, newSurface, 0);

        glActiveTexture(GL_TEXTURE0+position);
        glBindTexture(GL_TEXTURE_2D,tex_handl[position]);
        GLint mipmap_level = 0;
        GLint border       = 0;
        glTexImage2D(GL_TEXTURE_2D, // type of texture
                     mipmap_level,  //
                     GL_RGBA,       //color format in
                     surf->w,//texture weight
                     surf->h,//texture height
                     border,
                     GL_RGBA,       // color format out(color what we want to get in opengl)
                     GL_UNSIGNED_BYTE, // Specifies the data type of the texel data
                     newSurface->pixels);
        OM_GL_CHECK()
        //making correct color of zoomed texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        OM_GL_CHECK()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        OM_GL_CHECK()

        SDL_FreeSurface(surf);
        TTF_CloseFont(gFont);
        SDL_RenderClear(rend);

    }
    void createNameTetures(){
        for(int i=0;i<5;i++){
            /*createTextureName(
                        Characters[i].nameSTR,
                        i+1
                        );*/
            //Characters[i].anim.setSpeed(speed);

        }


        //get characters start positions
        /*for(auto i:Characters){
            std::cout<<i.posMat[3][0]<<":"<<
                       i.posMat[3][1]<<std::endl;
        }*/

        numbers_=new numbersText(12,10);

        numbers_->setPlace(body1,body2);

        //numbers_->info();


    }
    void renderGame(){
        //render background
//        activateProgBackground(0);
//        render_triangle(background1);
//        render_triangle(background2);

        //rotate background matrix
        rotateMat=glm::rotate(glm::mat4(1.0f),
                              glm::radians(static_cast<float>(-angle_new) ),
                              glm::vec3(0.0f,0.0f,1.0f));

        activateProgBody(0,rotateMat);
        render_triangle(background1);
        render_triangle(background2);

        buffMat=glm::translate(buffMat,glm::vec3(moveX,moveY,0.0f));

        activateProgBody(1,buffMat);
        render_triangle(body1);
        render_triangle(body2);

        //rock
        activateProgBackground(3);
        render_triangle(rock);

        checkCollision(body1,rock)=crash;
        checkCollision(body2,rock)=crash;


        //imgui part
        imgui_newframe(window);
        imgui_window(angle_new,water_power,crash);
        imgui_render();


        glGetError();

        swapBuffers();

    }    
    void logic(){


        if(angle_new!=angle_old){

            if( 0==angle_new || 360==angle_new){

                moveX=0.0f;
                moveY=water_power;

            }else if(angle_new>0 && angle_new<90){

                moveX=sin(angle_new * 3.14 / 180)*water_power;
                moveY=cos(angle_new * 3.14 / 180)*water_power;

            }else if(angle_new==90){

                moveX=water_power;
                moveY=0.0f;

            }else if(angle_new>90 && angle_new<180){
                moveX=cos( (angle_new-90) * 3.14 / 180)*water_power;
                moveY=sin( (angle_new-90) * 3.14 / 180)*water_power*(-1.0f);

            }else if(angle_new==180){

                moveX=0.0f;
                moveY=-water_power;
            }else if(angle_new>180 && angle_new<270){

                moveX=sin( (angle_new-180) * 3.14 / 180)*water_power*(-1.0f);
                moveY=cos( (angle_new-180) * 3.14 / 180)*water_power*(-1.0f);

            }else if(angle_new==270){

                moveX=-water_power;
                moveY=0.0f;

            }else if(angle_new>270 && angle_new<360){
                moveX=cos( (angle_new-270) * 3.14 / 180)*water_power*(-1.0f);
                moveY=sin( (angle_new-270) * 3.14 / 180)*water_power;

            }

            angle_old=angle_new;
        }

    }
    void startTimer(){
        timerTime=buffTimerTime*1000;
        if(timerTime>60000){
            minutes_=timerTime/60000;
            seconds_=(timerTime%minutes_)/1000;
        }else {
            minutes_=0;
            seconds_=timerTime/1000;
        }

        //std::cout<<std::to_string(timerTime)<<std::endl;
        //std::cout<<std::to_string(minutes_)<<":"<<std::to_string(seconds_)<<std::endl;

        startTime=SDL_GetTicks();
        //std::cout<<std::to_string(startTime)<<std::endl;

    }
};
        //tests for gl functions
        static const char* source_to_strv(GLenum source)
        {
            switch (source)
            {
                case GL_DEBUG_SOURCE_API:
                    return "API";
                case GL_DEBUG_SOURCE_SHADER_COMPILER:
                    return "SHADER_COMPILER";
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                    return "WINDOW_SYSTEM";
                case GL_DEBUG_SOURCE_THIRD_PARTY:
                    return "THIRD_PARTY";
                case GL_DEBUG_SOURCE_APPLICATION:
                    return "APPLICATION";
                case GL_DEBUG_SOURCE_OTHER:
                    return "OTHER";
            }
            return "unknown";
        }

        static const char* type_to_strv(GLenum type)
        {
            switch (type)
            {
                case GL_DEBUG_TYPE_ERROR:
                    return "ERROR";
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                    return "DEPRECATED_BEHAVIOR";
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                    return "UNDEFINED_BEHAVIOR";
                case GL_DEBUG_TYPE_PERFORMANCE:
                    return "PERFORMANCE";
                case GL_DEBUG_TYPE_PORTABILITY:
                    return "PORTABILITY";
                case GL_DEBUG_TYPE_MARKER:
                    return "MARKER";
                case GL_DEBUG_TYPE_PUSH_GROUP:
                    return "PUSH_GROUP";
                case GL_DEBUG_TYPE_POP_GROUP:
                    return "POP_GROUP";
                case GL_DEBUG_TYPE_OTHER:
                    return "OTHER";
            }
            return "unknown";
        }
        static const char* severity_to_strv(GLenum severity)
        {
            switch (severity)
            {
                case GL_DEBUG_SEVERITY_HIGH:
                    return "HIGH";
                case GL_DEBUG_SEVERITY_MEDIUM:
                    return "MEDIUM";
                case GL_DEBUG_SEVERITY_LOW:
                    return "LOW";
                case GL_DEBUG_SEVERITY_NOTIFICATION:
                    return "NOTIFICATION";
            }
            return "unknown";
        }

        static std::array<char, GL_MAX_DEBUG_MESSAGE_LENGTH> local_log_buff;

        static void APIENTRY callback_opengl_debug(
            GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
            const GLchar* message, [[maybe_unused]] const void* userParam)
        {
            // The memory formessageis owned and managed by the GL, and should onlybe
            // considered valid for the duration of the function call.The behavior of
            // calling any GL or window system function from within thecallback function
            // is undefined and may lead to program termination.Care must also be taken
            // in securing debug callbacks for use with asynchronousdebug output by
            // multi-threaded GL implementations.  Section 18.8 describes thisin further
            // detail.

            auto& buff{ local_log_buff };
            int   num_chars = std::snprintf(
                buff.data(), buff.size(), "%s %s %d %s %.*s\n", source_to_strv(source),
                type_to_strv(type), id, severity_to_strv(severity), length, message);

            if (num_chars > 0)
            {
                // TODO use https://en.cppreference.com/w/cpp/io/basic_osyncstream
                // to fix possible data races
                // now we use GL_DEBUG_OUTPUT_SYNCHRONOUS to garantie call in main
                // thread
                std::cerr.write(buff.data(), num_chars);
            }
        }
