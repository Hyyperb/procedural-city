#include <functional>
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

#include <limits.h>

#include <md5.h>

#include <stdexcept>
#include <vector>

#include <functional>

#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

// [x] Goal 1: get working fps controls
// [x] Goal 2: loop over chunks and meshes and render them
// [x] Goal 3: generate a city

const float CHUNK_SIZE = 30;
const float CELL_MARGIN = 0.2;

bool getRandomChunkEdge(int x, int y) {
  SetRandomSeed(std::hash<int>{}(x * 0x9e3779b9 ^ y + 234));
  int r = round(GetRandomValue(0, 100000) / 100000.0);
  printf("%d", r);
  float z = (stb_perlin_fbm_noise3(x, y, 1.0, 0.6, 1.2, 4));
  // printf("%f", z);
  return round(z);
}

class Chunk {
public:
  Vector2 coords;
  std::vector<Chunk> *otherChunks;

  float buildingHeight;

  struct DirectionsBool {
    bool xp;
    bool xn;
    bool zp;
    bool zn;

    bool hasAnyConnection() { return xp || xn || zp || zn; }

    void fromNumber(int id) {
      if (id > 15 || id < 0) {
        throw std::invalid_argument(
            "DirectionsBool::fromNumber(id)::id should be "
            "between 0 to 15 (inclusive)");
      }
      xp = id & 0b0001;
      xn = id & 0b0010;
      zp = id & 0b0100;
      zn = id & 0b1000;
    }

  } roadConnections;

  Chunk(int x, int y, std::vector<Chunk> *otherChunks) {
    coords.x = x * CHUNK_SIZE;
    coords.y = y * CHUNK_SIZE;

    this->otherChunks = otherChunks;

    // roadConnections.fromNumber(GetRandomValue(0, 15));

    // roadConnections(getChunkTileAt(x, y));

    roadConnections.xp = getRandomChunkEdge(x + 1, y);
    roadConnections.xn = getRandomChunkEdge(x, y);
    roadConnections.zp = getRandomChunkEdge(x, y + 1);
    roadConnections.zn = getRandomChunkEdge(x, y);
  }

  void draw() {
    for (int x = 0; x < 3; x++) {
      for (int z = 0; z < 3; z++) {
        if ((x == 1 && z == 1 && roadConnections.hasAnyConnection()) ||
            (x == 1 && z == 0 && roadConnections.xn) ||
            (x == 2 && z == 1 && roadConnections.xp) ||
            (x == 0 && z == 1 && roadConnections.zp) ||
            (x == 1 && z == 2 && roadConnections.zp)) {
          continue;
        }
        // SetRandomSeed(coords.x + coords.y * 334 + x * 241 + z * 523);
        //  buildingHeight = GetRandomValue(20, 80);
        SetRandomSeed(std::hash<int>{}(x * 0x9e3779b9 ^ (int)coords.y + 234));
        buildingHeight =
            (stb_perlin_fbm_noise3(x, z, 1.0, 0.8, 1.2, 3) + 1) * 50;

        Vector3 buildingPosition = {
            coords.x + x * CHUNK_SIZE / 3 + CHUNK_SIZE / 6, buildingHeight / 2,
            coords.y + z * CHUNK_SIZE / 3 + CHUNK_SIZE / 6};

        int buildingWidth = CHUNK_SIZE / 3 - CELL_MARGIN * 2;

        // building
        DrawCube(buildingPosition, buildingWidth, buildingHeight, buildingWidth,
                 RAYWHITE);

        // building wires
        DrawCubeWires(buildingPosition, buildingWidth, buildingHeight,
                      buildingWidth, BLACK);

        // pavement floor
        DrawPlane({coords.x + CHUNK_SIZE / 2, 0, coords.y + CHUNK_SIZE / 2},
                  {CHUNK_SIZE, CHUNK_SIZE}, GRAY);

        // roads
        DrawPlane({coords.x + CHUNK_SIZE / 2, 0.05, coords.y + CHUNK_SIZE / 2},
                  {CHUNK_SIZE / 3 - CELL_MARGIN, CHUNK_SIZE}, DARKGRAY);
        DrawPlane({coords.x + CHUNK_SIZE / 2, 0.05, coords.y + CHUNK_SIZE / 2},
                  {CHUNK_SIZE, CHUNK_SIZE / 3 - CELL_MARGIN}, DARKGRAY);
      }
    }
  }
};

int main() {
  Camera3D camera{{4, 2, 6}, {7, 1, 2}, {0, 1, 0}, 110.0, CAMERA_PERSPECTIVE};

  // Shader shader;
  // shader = LoadShader("vertex.glsl", "fragment.glsl");

  InitWindow(1920, 1080, "FPS controls");

  DisableCursor();
  SetTargetFPS(60);

  std::vector<Chunk> chunks;

  for (int x = -10; x < 10; x++) {
    for (int y = -10; y < 10; y++) {
      chunks.push_back(Chunk(x, y, &chunks));
    }
  }

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);
    BeginDrawing();
    ClearBackground(SKYBLUE);
    BeginMode3D(camera);

    for (Chunk chunk : chunks) {
      chunk.draw();
    }
    EndMode3D();

    DrawFPS(5, 5);

    EndDrawing();
    // BeginShaderMode(shader);
    // EndShaderMode();
  }

  CloseWindow();

  return 0;
}
