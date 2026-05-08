#include "Mesh.h"

std::vector<Mesh*> Mesh::meshes;
unsigned int Mesh::shaderProgram = 0;

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
  
  linearVelocity -= glm::vec3(0.0f, 0.025f, 0.0f);

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

  int fbWidth, fbHeight;
      glfwGetFramebufferSize(Window::window, &fbWidth, &fbHeight); 

  // ===== MODEL (object transform) =====
  glm::mat4 model = glm::mat4(1.0f);

  // local rotation
  model = glm::translate(model, position);
  if (glm::length(angularVelocity) > 0.0001f) {
    model = glm::rotate(
      model,
      time,
      glm::normalize(angularVelocity)
    );
  }

  // ===== VIEW (camera) =====
  glm::mat4 view = glm::lookAt(
      Camera::position,
      Camera::position + Camera::forward,
      glm::vec3(0.0f, 1.0f, 0.0f)
  );

  glm::mat4 projection = glm::perspective(
      glm::radians(70.0f),
      (float)fbWidth / fbHeight,
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

  // ===== LIGHT =====
  glUniform3f(
      glGetUniformLocation(shaderProgram, "lightDir"),
      -0.5f, 1.0f, -0.3f
  );

  // ===== TEXTURE =====
  glUniform1i(
    glGetUniformLocation(shaderProgram, "useTexture"),
    hasTexture ? 1 : 0
  );

  if (hasTexture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(
      glGetUniformLocation(shaderProgram, "texture1"),
      0
    );
  }

  glUniform3f(
      glGetUniformLocation(shaderProgram, "color"),
      color.x, color.y, color.z    
  );

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
