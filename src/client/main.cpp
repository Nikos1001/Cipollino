
#include <stdio.h>
#include "../platform/platform.h"
#include "../render/mesh.h"
#include "../render/shader.h"

class Client : public App {
    void init() {
        mesh.init(1, (int[]){2});
        float verts[] = {
            0.0f, 0.5f,
            -0.5f, -0.5f,
            0.5f, -0.5f
        };
        unsigned int idxs[] = {
            0, 1, 2
        };
        mesh.verts = 3;
        mesh.vertData = verts;
        mesh.tris = 1;
        mesh.idxs = idxs;
        mesh.upload();

        shader.init(R"(
            #version 100 

            attribute vec2 aPos;

            void main() {
                gl_Position = vec4(aPos, 0.0, 1.0);                
            }

        )", R"(
            #version 100 

            void main() {
                
                gl_FragColor = vec4(1.0);

            }
        )");

        sock.init("ws://localhost:2000");
        killedSock = false;
    }

    void tick(float dt) {
        ImGui::Begin("Hello!");
        ImGui::Text("DT: %g", dt);
        ImGui::End();

        shader.use();
        mesh.render();

        if(glfwGetTime() > 3 && !killedSock) {
            killedSock = true; 
            sock.free();
        }
        if(!killedSock) {
            if(sock.ready())
                sock.send((void*)"Hel\0lo", 6);
            
            SocketMsg msg = sock.nextMsg();
            if(msg.valid()) {
                printf("%d\n", (int)msg.size);
                msg.free();
            }
        }
    }

    Mesh mesh;
    Shader shader;
    Socket sock;
    bool killedSock;

};

int main() {
    Client client;
    runApp(&client); 
}
