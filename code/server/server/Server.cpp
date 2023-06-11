#include "server/Server.hpp"

NRenderer::Server& getServer() {
    static NRenderer::Server b{};
    return b;
}