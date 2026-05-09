#include "Mesh.h"
#include "Light.h"

std::vector<Mesh*> Mesh::meshes;
unsigned int Mesh::shaderProgram, Mesh::shadowShaderProgram, Mesh::depthMap, Mesh::depthMapFBO = 0;
const unsigned int Mesh::SHADOW_WIDTH = 4096;
const unsigned int Mesh::SHADOW_HEIGHT = 4096;
glm::mat4 Mesh::lightView, Mesh::lightProjection, Mesh::lightSpaceMatrix;
glm::vec3 Mesh::lightPos(-50.0f, 50.0f, -50.0f);

void Mesh::drawAllShadows() {
  // ===== LIGHT SPACE MATRIX (SHADOWS) =====
  // You should compute this once per frame ideally, but this works for now
  lightProjection = glm::ortho(
    -100.0f, 100.0f,
    -100.0f, 100.0f,
    1.0f, 200.0f
  );

  lightView = glm::lookAt(
      lightPos,
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f)
  );

  lightSpaceMatrix = lightProjection * lightView;

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

  glClear(GL_DEPTH_BUFFER_BIT);
  glUseProgram(shadowShaderProgram);

  glPolygonOffset(2.0f, 4.0f);
  glEnable(GL_POLYGON_OFFSET_FILL);

  glUniformMatrix4fv(
    glGetUniformLocation(shadowShaderProgram, "lightSpaceMatrix"),
    1,
    GL_FALSE,
    glm::value_ptr(lightSpaceMatrix)
  );

  for (Mesh* mesh : meshes)
  {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, mesh->position);

    /*model = glm::rotate(
      model,
      glm::radians(mesh->rotation.x),
      glm::vec3(1.0f, 0.0f, 0.0f)
    );

    model = glm::rotate(
      model,
      glm::radians(mesh->rotation.y),
      glm::vec3(0.0f, 1.0f, 0.0f)
    );

    model = glm::rotate(
      model,
      glm::radians(mesh->rotation.z),
      glm::vec3(0.0f, 0.0f, 1.0f)
    );

    model = glm::scale(model, mesh->scale);*/

    glUniformMatrix4fv(
      glGetUniformLocation(shadowShaderProgram, "model"),
      1,
      GL_FALSE,
      glm::value_ptr(model)
    );

    glBindVertexArray(mesh->VAO);

    glDrawArrays(
      GL_TRIANGLES,
      0,
      mesh->vertexCount
    );
  }

  glBindVertexArray(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0); 
  glViewport(0, 0, Window::fbWidth, Window::fbHeight);
}

void Mesh::drawAllMeshes() {
  for (Mesh* mesh : meshes) {
    mesh->draw();
  }
}

void Mesh::calculateAllPhysics() {
  for (Mesh* mesh : meshes) {
    mesh->calculatePhysics();
  }
}

void Mesh::initShader() {
  // already initialized
  if (shaderProgram != 0)
      return;

  std::string vertexCode =
      FileLoader::loadFile("shaders/vertex.glsl");

  std::string fragmentCode =
      FileLoader::loadFile("shaders/fragment.glsl");

  const char* vertexShaderSource = vertexCode.c_str();
  const char* fragmentShaderSource = fragmentCode.c_str();

  // ===== VERTEX SHADER =====
  unsigned int vertexShader =
      glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(
      vertexShader,
      1,
      &vertexShaderSource,
      NULL
  );

  glCompileShader(vertexShader);

  int success;
  char infoLog[512];

  glGetShaderiv(
      vertexShader,
      GL_COMPILE_STATUS,
      &success
  );

  if (!success) {
      glGetShaderInfoLog(
          vertexShader,
          512,
          NULL,
          infoLog
      );

      std::cout
          << "VERTEX SHADER ERROR:\n"
          << infoLog
          << std::endl;
  }

  // ===== FRAGMENT SHADER =====
  unsigned int fragmentShader =
      glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(
      fragmentShader,
      1,
      &fragmentShaderSource,
      NULL
  );

  glCompileShader(fragmentShader);

  glGetShaderiv(
      fragmentShader,
      GL_COMPILE_STATUS,
      &success
  );

  if (!success) {
      glGetShaderInfoLog(
          fragmentShader,
          512,
          NULL,
          infoLog
      );

      std::cout
          << "FRAGMENT SHADER ERROR:\n"
          << infoLog
          << std::endl;
  }

  // ===== SHADER PROGRAM =====
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);

  glGetProgramiv(
      shaderProgram,
      GL_LINK_STATUS,
      &success
  );

  if (!success) {
      glGetProgramInfoLog(
          shaderProgram,
          512,
          NULL,
          infoLog
      );

      std::cout
          << "PROGRAM LINK ERROR:\n"
          << infoLog
          << std::endl;
  }

  // cleanup
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // shadows
  vertexCode =
      FileLoader::loadFile("shaders/shadowVertex.glsl");

  fragmentCode =
      FileLoader::loadFile("shaders/shadowFragment.glsl");

  vertexShaderSource = vertexCode.c_str();
  fragmentShaderSource = fragmentCode.c_str();

  // ===== VERTEX SHADER =====
  vertexShader =
      glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(
      vertexShader,
      1,
      &vertexShaderSource,
      NULL
  );

  glCompileShader(vertexShader);

  glGetShaderiv(
      vertexShader,
      GL_COMPILE_STATUS,
      &success
  );

  if (!success) {
      glGetShaderInfoLog(
          vertexShader,
          512,
          NULL,
          infoLog
      );

      std::cout
          << "VERTEX SHADER ERROR:\n"
          << infoLog
          << std::endl;
  }

  // ===== FRAGMENT SHADER =====
  fragmentShader =
      glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(
      fragmentShader,
      1,
      &fragmentShaderSource,
      NULL
  );

  glCompileShader(fragmentShader);

  glGetShaderiv(
      fragmentShader,
      GL_COMPILE_STATUS,
      &success
  );

  if (!success) {
      glGetShaderInfoLog(
          fragmentShader,
          512,
          NULL,
          infoLog
      );

      std::cout
          << "FRAGMENT SHADER ERROR:\n"
          << infoLog
          << std::endl;
  }

  // ===== SHADER PROGRAM =====
  shadowShaderProgram = glCreateProgram();

  glAttachShader(shadowShaderProgram, vertexShader);
  glAttachShader(shadowShaderProgram, fragmentShader);

  glLinkProgram(shadowShaderProgram);

  glGetProgramiv(
      shadowShaderProgram,
      GL_LINK_STATUS,
      &success
  );

  if (!success) {
      glGetProgramInfoLog(
          shadowShaderProgram,
          512,
          NULL,
          infoLog
      );

      std::cout
          << "PROGRAM LINK ERROR:\n"
          << infoLog
          << std::endl;
  }

  // cleanup
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glGenFramebuffers(1, &depthMapFBO);

  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);

  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_DEPTH_COMPONENT,
      SHADOW_WIDTH,
      SHADOW_HEIGHT,
      0,
      GL_DEPTH_COMPONENT,
      GL_FLOAT,
      NULL
  );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  float borderColor[] = {1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

  glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_DEPTH_ATTACHMENT,
      GL_TEXTURE_2D,
      depthMap,
      0
  );

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  std::cout << "Shader initialized\n";
}

Mesh::Mesh(float* vertices, size_t size, glm::vec3 angularVelocity, glm::vec3 position, glm::vec3 color, bool anchored) 
  : angularVelocity(angularVelocity), linearVelocity(glm::vec3(0.0f, 0.0f, 0.0f)), hasTexture(false), position(position), color(color), anchored(anchored) {
  init(vertices, size);
}

Mesh::Mesh(float* vertices, size_t size, std::string texPath, glm::vec3 angularVelocity, glm::vec3 position, bool anchored) 
  : angularVelocity(angularVelocity), linearVelocity(glm::vec3(0.0f, 0.0f, 0.0f)), hasTexture(true), position(position), anchored(anchored), texture(FileLoader::loadTexture(texPath)) {
  init(vertices, size);
}

Mesh::Mesh(std::string path, std::string texPath, glm::vec3 angularVelocity, glm::vec3 position, bool anchored) 
  : angularVelocity(angularVelocity), linearVelocity(glm::vec3(0.0f, 0.0f, 0.0f)), position(position), hasTexture(true), anchored(anchored), texture(FileLoader::loadTexture(texPath)) {
  std::vector<float> verticesArray = getVerticesFromFile(path);
  float* vertices = verticesArray.data();
  size_t size = verticesArray.size() * sizeof(float);

  init(vertices, size);
}

Mesh::Mesh(std::string path, glm::vec3 angularVelocity, glm::vec3 position, glm::vec3 color, bool anchored) 
  : angularVelocity(angularVelocity), linearVelocity(glm::vec3(0.0f, 0.0f, 0.0f)), position(position), hasTexture(false), color(color), anchored(anchored) {
  std::vector<float> verticesArray = getVerticesFromFile(path);
  float* vertices = verticesArray.data();
  size_t size = verticesArray.size() * sizeof(float);

  init(vertices, size);
}

void Mesh::init(float* vertices, size_t size) {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

  // 3 pos + 3 normal + 2 uv = 8 floats per vertex
  vertexCount = size / (8 * sizeof(float));

  // POSITION
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // NORMAL
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // UV
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  computeCollider(vertices, size);

  meshes.push_back(this);
}

void Mesh::computeCollider(float* vertices, size_t size) {
  if (vertices == nullptr || size == 0) return;

  if (size % (8 * sizeof(float)) != 0) {
    std::cout << "invalid vertex buffer size\n";
    return;
  }

  colliderMin = glm::vec3(std::numeric_limits<float>::max());
  colliderMax = glm::vec3(std::numeric_limits<float>::lowest());

  size_t floatCount = size / sizeof(float);

  for (size_t i = 0; i + 2 < floatCount; i += 8) {
    glm::vec3 v(
      vertices[i],
      vertices[i + 1],
      vertices[i + 2]
    );

    colliderMin = glm::min(colliderMin, v);
    colliderMax = glm::max(colliderMax, v);
  }

  colliderSize = colliderMax - colliderMin;
}

std::vector<float> Mesh::getVerticesFromFile(std::string path) {
  Assimp::Importer importer;
  std::vector<float> vertices;

  const aiScene* scene = importer.ReadFile(
    path,
    aiProcess_Triangulate |
    aiProcess_GenNormals |
    aiProcess_FlipUVs
  );

  if (!scene || !scene->mRootNode) {
    std::cout << importer.GetErrorString() << std::endl;
    return vertices;
  }

  aiMesh* mesh = scene->mMeshes[0];

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];

    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      unsigned int index = face.mIndices[j];

      // POSITION
      vertices.push_back(mesh->mVertices[index].x);
      vertices.push_back(mesh->mVertices[index].y);
      vertices.push_back(mesh->mVertices[index].z);

      // NORMAL
      vertices.push_back(mesh->mNormals[index].x);
      vertices.push_back(mesh->mNormals[index].y);
      vertices.push_back(mesh->mNormals[index].z);

      // UV (SAFE fallback if missing)
      if (mesh->HasTextureCoords(0)) {
        vertices.push_back(mesh->mTextureCoords[0][index].x);
        vertices.push_back(mesh->mTextureCoords[0][index].y);
      } else {
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
      }
    }
  }

  return vertices;
}

bool Mesh::intersects(Mesh* other) {
  glm::vec3 aMin = position + colliderMin;
  glm::vec3 aMax = position + colliderMax;

  glm::vec3 bMin = other->position - other->colliderSize * 0.5f;
  glm::vec3 bMax = other->position + other->colliderSize * 0.5f;

  return (
      aMin.x <= bMax.x &&
      aMax.x >= bMin.x &&

      aMin.y <= bMax.y &&
      aMax.y >= bMin.y &&

      aMin.z <= bMax.z &&
      aMax.z >= bMin.z
  );
}

glm::vec3 Mesh::resolveAABB(Mesh* a, Mesh* b) {
    glm::vec3 aMin = a->position + a->colliderMin;
    glm::vec3 aMax = a->position + a->colliderMax;

    glm::vec3 bMin = b->position + b->colliderMin;
    glm::vec3 bMax = b->position + b->colliderMax;

    glm::vec3 overlap(
        std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x),
        std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y),
        std::min(aMax.z, bMax.z) - std::max(aMin.z, bMin.z)
    );

    // smallest axis = separation axis
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        return glm::vec3((a->position.x < b->position.x) ? -overlap.x : overlap.x, 0, 0);
    }
    if (overlap.y < overlap.z) {
        return glm::vec3(0, (a->position.y < b->position.y) ? -overlap.y : overlap.y, 0);
    }
    return glm::vec3(0, 0, (a->position.z < b->position.z) ? -overlap.z : overlap.z);
}

void Mesh::calculatePhysics() {
  if (anchored) return;
  
  linearVelocity -= glm::vec3(0.0f, 25.0f, 0.0f) * (float)Window::deltaTime;

  glm::vec3 oldPos = position;

  glm::vec3 delta = linearVelocity;

  position += linearVelocity * (float)Window::deltaTime;

  for (Mesh* other : meshes) {
    if (other == this) continue;

    if (intersects(other)) {
      glm::vec3 correction = resolveAABB(this, other);

      position += correction;

      if (correction.y != 0) linearVelocity.y = 0;
      if (correction.x != 0) linearVelocity.x = 0;
      if (correction.z != 0) linearVelocity.z = 0;
    }
  }
}

void Mesh::draw() {
  glUseProgram(shaderProgram);

  float time = (float)glfwGetTime();

  // ===== MODEL =====
  glm::mat4 model = glm::mat4(1.0f);

  model = glm::translate(model, position);

  if (glm::length(angularVelocity) > 0.0001f)
  {
      model = glm::rotate(
          model,
          time,
          glm::normalize(angularVelocity)
      );
  }

  // ===== VIEW =====
  glm::mat4 view = glm::lookAt(
      Camera::position,
      Camera::position + Camera::forward,
      glm::vec3(0.0f, 1.0f, 0.0f)
  );

  // ===== PROJECTION =====
  glm::mat4 projection = glm::perspective(
      glm::radians(70.0f),
      (float)Window::fbWidth / (float)Window::fbHeight,
      0.1f,
      100.0f
  );

  // ===== SEND MATRICES =====
  glUniformMatrix4fv(
      glGetUniformLocation(shaderProgram, "model"),
      1,
      GL_FALSE,
      glm::value_ptr(model)
  );

  glUniformMatrix4fv(
      glGetUniformLocation(shaderProgram, "view"),
      1,
      GL_FALSE,
      glm::value_ptr(view)
  );

  glUniformMatrix4fv(
      glGetUniformLocation(shaderProgram, "projection"),
      1,
      GL_FALSE,
      glm::value_ptr(projection)
  );

  glUniformMatrix4fv(
      glGetUniformLocation(shaderProgram, "lightSpaceMatrix"),
      1,
      GL_FALSE,
      glm::value_ptr(lightSpaceMatrix)
  );

  // ===== LIGHT =====
  glUniform1i(glGetUniformLocation(shaderProgram, "lightCount"), Light::lights.size());

  for (int i = 0; i < Light::lights.size(); i++) {
    if (!Light::lights[i]->enabled) continue;

    glUniform3f(glGetUniformLocation(shaderProgram, ("lightPos[" + std::to_string(i) + "]").c_str()),
        Light::lights[i]->position.x,
        Light::lights[i]->position.y,
        Light::lights[i]->position.z
    );

    glUniform3f(glGetUniformLocation(shaderProgram, ("lightColor[" + std::to_string(i) + "]").c_str()),
        Light::lights[i]->color.x,
        Light::lights[i]->color.y,
        Light::lights[i]->color.z
    );
  }
  
  // ===== SHADOW MAP =====
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, depthMap);

  glUniform1i(
      glGetUniformLocation(shaderProgram, "shadowMap"),
      1
  );

  // ===== TEXTURE FLAG =====
  glUniform1i(
      glGetUniformLocation(shaderProgram, "useTexture"),
      hasTexture ? 1 : 0
  );

  if (hasTexture)
  {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);

      glUniform1i(
          glGetUniformLocation(shaderProgram, "texture1"),
          0
      );
  }

  // ===== COLOR =====
  glUniform3f(
      glGetUniformLocation(shaderProgram, "color"),
      color.x, color.y, color.z
  );

  // ===== DRAW =====
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
