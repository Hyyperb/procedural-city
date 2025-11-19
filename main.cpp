#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

#include <limits.h>

#include <md5.h>

#include <stdexcept>
#include <vector>

// [x] Goal 1: get working fps controls
// [x] Goal 2: loop over chunks and meshes and render them
// [ ] Goal 3: generate a city

const float CHUNK_SIZE = 30;
const float CELL_MARGIN = 0.2;

// class WaveFunctionCollapse {
// public:
//   int seed;
//   WaveFunctionCollapse(int seed) { this->seed = seed; }
//   WaveFunctionCollapse()
//       : WaveFunctionCollapse(GetRandomValue(INT_MIN, INT_MAX)) {};
// };

class Chunk {
public:
  Vector2 coords;
  unsigned int seedhash;
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

  } roadConnections, adjacentRoadConntions;

  Chunk(int x, int y, std::vector<Chunk> *otherChunks) {
    coords.x = x * CHUNK_SIZE;
    coords.y = y * CHUNK_SIZE;

    this->otherChunks = otherChunks;

    roadConnections.fromNumber(GetRandomValue(0, 15));

    // roadConnections(getChunkTileAt(x, y));

    // roadConnections.xp = (bool)GetRandomValue(0, 0);
    // roadConnections.xn = (bool)GetRandomValue(0, 0);
    // roadConnections.zp = (bool)GetRandomValue(0, 0);
    // roadConnections.zn = (bool)GetRandomValue(0, 0);
  }

  // TODO: Replace this with a good hash function;
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
        SetRandomSeed(coords.x + coords.y * 334 + x * 241 + z * 523);
        buildingHeight = GetRandomValue(20, 30);

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
        // DrawPlane({coords.x + CHUNK_SIZE / 2, 1, coords.y + CHUNK_SIZE / 2},
        //           {CHUNK_SIZE / 3 - CELL_MARGIN, CHUNK_SIZE}, BLACK);
        // DrawPlane({coords.x + CHUNK_SIZE / 2, 1, coords.y + CHUNK_SIZE / 2},
        //           {CHUNK_SIZE, CHUNK_SIZE / 3 - CELL_MARGIN}, BLACK);
      }
    }
  }
};

int main() {
  Camera3D camera{
      {0, 40, 0}, {0, 10, -30}, {0, 1, 0}, 80.0, CAMERA_PERSPECTIVE};

  // Shader shader;
  // shader = LoadShader("vertex.glsl", "fragment.glsl");

  InitWindow(1920, 1080, "FPS controls");

  DisableCursor();
  SetTargetFPS(60);

  // Model model = LoadModel("chadsuzane.glb");
  // // Texture2D texture = LoadTexture("texture.png");
  // // model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
  // Vector2 position[30];

  // for (int i = 0; i < 10; i++) {
  //   position[i].x = GetRandomValue(-40, 40);
  //   position[i].y = GetRandomValue(-40, 40);
  // }

  // BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);

  std::vector<Chunk> chunks;

  for (int x = -10; x < 10; x++) {
    for (int y = -10; y < 10; y++) {
      chunks.push_back(Chunk(x, y, &chunks));
    }
  }

  std::cout << "here";

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);
    BeginDrawing();
    ClearBackground(SKYBLUE);
    BeginMode3D(camera);

    for (int i = 0; i < 30; i++) {
      // Vector2MoveTowards(position[i], {camera.position.x, camera.position.y},
      // 0.01); DrawModel(model, {position[i].x, 0, position[i].y}, 1.0f,
      // WHITE); DrawModelWires(model, {position[i].x, 0, position[i].y}, 1.0f,
      // BLACK);
    }

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
