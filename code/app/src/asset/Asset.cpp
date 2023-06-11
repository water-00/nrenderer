#include "asset/Asset.hpp"

namespace NRenderer
{
    void Asset::genPreviewGlBuffersPerNode(NodeItem& node) {
        if (node.glVAO != 0) {
            glDeleteVertexArrays(1, &node.glVAO);
            node.glVAO = 0;
        }
        if (node.glVBO != 0) {
            glDeleteBuffers(1, &node.glVBO);
            node.glVBO = 0;
        }
        if (node.glEBO != 0) {
            glDeleteBuffers(1, &node.glEBO);
            node.glEBO = 0;
        }

        glGenVertexArrays(1, &node.glVAO);
        glBindVertexArray(node.glVAO);
        auto& np = *node.node;
        using T = Node::Type;
        if (np.type == T::TRIANGLE) {
            auto& t = *triangles[np.entity];
            glGenBuffers(1, &node.glVBO);

            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*3, &t.v[0], GL_DYNAMIC_DRAW);
            
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::SPHERE) {
            auto&s = *spheres[np.entity];
            node.externalDrawData = SharedGlDrawData{new GlDrawData{}};
            auto& bf = node.externalDrawData->positions;
            const int count = 60;
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , s.radius*sin(float(i)/float(count)*6.28) , 0});
                bf.push_back(p);
            }
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            for (int i=0; i<=count/4; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{0, s.radius*sin(float(i)/float(count)*6.28) , s.radius*cos(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            glGenBuffers(1, &node.glVBO);

            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*bf.size(), &bf[0], GL_DYNAMIC_DRAW);
            
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::PLANE) {
            auto&p = *planes[np.entity];
            node.externalDrawData = make_shared<GlDrawData>();
            auto& bf = node.externalDrawData->positions;
            bf.push_back(p.position);
            bf.push_back(p.position + p.u);
            bf.push_back(p.position + p.u + p.v);
            bf.push_back(p.position + p.v);
            glGenBuffers(1, &node.glVBO);

            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*bf.size(), &bf[0], GL_DYNAMIC_DRAW);
            
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::MESH) {
            auto& m = *meshes[np.entity];
            glGenBuffers(1, &node.glVBO);
            glGenBuffers(1, &node.glEBO);

            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*m.positions.size(), &m.positions[0], GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, node.glEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* m.positionIndices.size(), &m.positionIndices[0], GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            // glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        glBindVertexArray(0);
        
    }

    void Asset::genPreviewGlBuffersPerLight(LightItem& light) {
        if (light.glVAO != 0) {
            glDeleteVertexArrays(1, &light.glVAO);
            light.glVAO = 0;
        }
        if (light.glVBO != 0) {
            glDeleteBuffers(1, &light.glVBO);
            light.glVBO = 0;
        }
        if (light.glEBO != 0) {
            glDeleteBuffers(1, &light.glEBO);
            light.glEBO = 0;
        }
        auto& lp = *light.light;
        using T = Light::Type;
        light.externalDrawData = make_shared<GlDrawData>();
        auto& ps = light.externalDrawData->positions;
        glGenVertexArrays(1, &light.glVAO);
        glBindVertexArray(light.glVAO);
        if (lp.type == T::POINT) {
            const float l = 1.f/20.f;
            const int count = 8;
            for (int i=0; i < count; i++) {
                Vec3 p{l*cos(float(i)/float(count)*2*3.1415926f), l*sin(float(i)/float(count)*2*3.1415926f), 0};
                ps.push_back({0, 0, 0});
                ps.push_back(p);
            }
            glGenBuffers(1, &light.glVBO);

            glBindBuffer(GL_ARRAY_BUFFER, light.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*ps.size(), &ps[0], GL_STATIC_DRAW);
            
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (lp.type == T::AREA) {
            ps.assign(4, {});
            auto& a = *areaLights[lp.entity];
            ps[0] = {0, 0, 0};
            ps[1] = ps[0] + a.u;
            ps[2] = ps[1] + a.v;
            ps[3] = ps[0] + a.v;

            glGenBuffers(1, &light.glVBO);

            glBindBuffer(GL_ARRAY_BUFFER, light.glVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*ps.size(), &ps[0], GL_DYNAMIC_DRAW);
            
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void *)0);
        
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        }
        else if (lp.type == T::DIRECTIONAL) {

        }
        else if (lp.type == T::SPOT) {

        }
        glBindVertexArray(0);
    }

    void Asset::updateNodeGlDrawData(NodeItem& node) {
        auto& np = *node.node;
        using T = Node::Type;
        if (np.type == T::SPHERE) {
            node.externalDrawData = make_shared<GlDrawData>();
            auto&s = *spheres[np.entity];
            node.externalDrawData = SharedGlDrawData{new GlDrawData{}};
            auto& bf = node.externalDrawData->positions;
            const int count = 60;
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , s.radius*sin(float(i)/float(count)*6.28) , 0});
                bf.push_back(p);
            }
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            for (int i=0; i<=count/4; i++) {
                Vec3 p = (s.position + Vec3{s.radius*cos(float(i)/float(count)*6.28) , 0, s.radius*sin(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            for (int i=0; i<=count; i++) {
                Vec3 p = (s.position + Vec3{0, s.radius*sin(float(i)/float(count)*6.28) , s.radius*cos(float(i)/float(count)*6.28)});
                bf.push_back(p);
            }
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*bf.size(), &bf[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::PLANE) {
            auto&p = *planes[np.entity];
            node.externalDrawData = make_shared<GlDrawData>();
            auto& bf = node.externalDrawData->positions;
            bf.push_back(p.position);
            bf.push_back(p.position + p.u);
            bf.push_back(p.position + p.u + p.v);
            bf.push_back(p.position + p.v);
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*bf.size(), &bf[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (np.type == T::TRIANGLE) {
            auto& t = *triangles[np.entity];
            glBindBuffer(GL_ARRAY_BUFFER, node.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*3, &t.v[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    void Asset::updateLightGlDrawData(LightItem& light) {
        auto& lp = *light.light;
        using T = Light::Type;
        auto& ps = light.externalDrawData->positions;
        if (lp.type == T::POINT) {
            
        }
        else if (lp.type == T::AREA) {
            light.externalDrawData = make_shared<GlDrawData>();
            ps.assign(4, {});
            auto& a = *areaLights[lp.entity];
            ps[0] = {0, 0, 0};
            ps[1] = ps[0] + a.u;
            ps[2] = ps[1] + a.v;
            ps[3] = ps[0] + a.v;
            glBindBuffer(GL_ARRAY_BUFFER, light.glVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3)*ps.size(), &ps[0]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else if (lp.type == T::DIRECTIONAL) {

        }
        else if (lp.type == T::SPOT) {

        }
    }
}