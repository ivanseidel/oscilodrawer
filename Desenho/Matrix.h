#define Matrix4x4Identity {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}

typedef float Matrix4x4[4][4];
typedef float Vector4[4];

void multiply4x4(Matrix4x4 a, Matrix4x4 b, Matrix4x4 matrix) {
  float a00=a[0][0];
  float a01=a[0][1];
  float a02=a[0][2];
  float a03=a[0][3];

  float a10=a[1][0];
  float a11=a[1][1];
  float a12=a[1][2];
  float a13=a[1][3];

  float a20=a[2][0];
  float a21=a[2][1];
  float a22=a[2][2];
  float a23=a[2][3];

  float a30=a[3][0];
  float a31=a[3][1];
  float a32=a[3][2];
  float a33=a[3][3];

  float b00=b[0][0];
  float b01=b[0][1];
  float b02=b[0][2];
  float b03=b[0][3];

  float b10=b[1][0];
  float b11=b[1][1];
  float b12=b[1][2];
  float b13=b[1][3];

  float b20=b[2][0];
  float b21=b[2][1];
  float b22=b[2][2];
  float b23=b[2][3];

  float b30=b[3][0];
  float b31=b[3][1];
  float b32=b[3][2];
  float b33=b[3][3];

  matrix[0][0] = a00*b00+a10*b01+a20*b02+a30*b03;
  matrix[0][1] = a01*b00+a11*b01+a21*b02+a31*b03;
  matrix[0][2] = a02*b00+a12*b01+a22*b02+a32*b03;
  matrix[0][3] = a03*b00+a13*b01+a23*b02+a33*b03;

  matrix[1][0] = a00*b10+a10*b11+a20*b12+a30*b13;
  matrix[1][1] = a01*b10+a11*b11+a21*b12+a31*b13;
  matrix[1][2] = a02*b10+a12*b11+a22*b12+a32*b13;
  matrix[1][3] = a03*b10+a13*b11+a23*b12+a33*b13;

  matrix[2][0] = a00*b20+a10*b21+a20*b22+a30*b23;
  matrix[2][1] = a01*b20+a11*b21+a21*b22+a31*b23;
  matrix[2][2] = a02*b20+a12*b21+a22*b22+a32*b23;
  matrix[2][3] = a03*b20+a13*b21+a23*b22+a33*b23;

  matrix[3][0] = a00*b30+a10*b31+a20*b32+a30*b33;
  matrix[3][1] = a01*b30+a11*b31+a21*b32+a31*b33;
  matrix[3][2] = a02*b30+a12*b31+a22*b32+a32*b33;
  matrix[3][3] = a03*b30+a13*b31+a23*b32+a33*b33;
}

void multiply4x4Point(Matrix4x4 a, Vector4 point, Vector4 out) {
  float a00=a[0][0];
  float a01=a[0][1];
  float a02=a[0][2];
  float a03=a[0][3];

  float a10=a[1][0];
  float a11=a[1][1];
  float a12=a[1][2];
  float a13=a[1][3];

  float a20=a[2][0];
  float a21=a[2][1];
  float a22=a[2][2];
  float a23=a[2][3];

  float a30=a[3][0];
  float a31=a[3][1];
  float a32=a[3][2];
  float a33=a[3][3];

  float x = point[0];
  float y = point[1];
  float z = point[2];
  float w = point[3];

  out[0] = x * a00 + y * a01 + z * a02 + w * a03;
  out[1] = x * a10 + y * a11 + z * a12 + w * a13;
  out[2] = x * a20 + y * a21 + z * a22 + w * a23;
  out[3] = x * a30 + y * a31 + z * a32 + w * a33;
  // out[3] = 1;
  // float w = x * a30 + y * a31 + z * a32 + /*w * */ a33;

  w = out[3];

  if (w != 1.0) { 
    out[0] /= w;
    out[1] /= w;
    out[2] /= w;
  //   // out[3] /= w;
  }
}

void identity(Matrix4x4 out) {
  memset(out, 0, 16);
  out[0][0] = 1;
  out[1][1] = 1;
  out[2][2] = 1;
  out[3][3] = 1;
}

void printMatrix(Matrix4x4 m) {
  Serial.println();
  for (int i = 0; i < 4; i++) {
    Serial.print("[");
    for (int j = 0; j < 4; j++) {
      Serial.print(j > 0 ? ", " : "");
      Serial.print(m[i][j]);
    }
    Serial.println("]");
  }
}

void printVector4(Vector4 p) {
  // Serial.println();
  Serial.print("[");
  for (int j = 0; j < 4; j++) {
    Serial.print(j > 0 ? ", " : "");
    Serial.print(p[j]);
  }
  Serial.println("]");
}

void translate(Matrix4x4 mat, Vector4 trans, Matrix4x4 out) {
  Matrix4x4 t = {
    {   1,   0,   0, trans[0]},
    {   0,   1,   0, trans[1]},
    {   0,   0,   1, trans[2]},
    {   0,   0,   0,        1}
  };
  multiply4x4(mat, t, out);
}

void rotateX(Matrix4x4 mat, float t, Matrix4x4 out) {
  float st = sin(t);
  float ct = cos(t);
  Matrix4x4 rot = {
    {   1,   0,   0,   0},
    {   0,  ct, -st,   0},
    {   0,  st,  ct,   0},
    {   0,   0,   0,   1}
  };
  multiply4x4(mat, rot, out);
}

void rotateY(Matrix4x4 mat, float t, Matrix4x4 out) {
  float st = sin(t);
  float ct = cos(t);
  Matrix4x4 rot = {
    {  ct,   0,  st,   0},
    {   0,   1,   0,   0},
    { -st,   0,  ct,   0},
    {   0,   0,   0,   1}
  };
  multiply4x4(mat, rot, out);
}

void rotateZ(Matrix4x4 mat, float t, Matrix4x4 out) {
  float st = sin(t);
  float ct = cos(t);
  Matrix4x4 rot = {
    {  ct, -st,   0,   0},
    {  st,  ct,   0,   0},
    {   0,   0,   1,   0},
    {   0,   0,   0,   1}
  };
  // printMatrix(rot);
  multiply4x4(mat, rot, out);
}

void setProjectionMatrix(float angleOfView, float near, float far, Matrix4x4 M) {
  // set the basic projection matrix
  float scale = 1 / tan(angleOfView * 0.5 * M_PI / 180); 
  M[0][0] = scale; // scale the x coordinates of the projected point 
  M[1][1] = scale; // scale the y coordinates of the projected point 
  M[2][2] = -far / (far - near); // used to remap z to [0,1] 
  M[2][3] = -far * near / (far - near); // used to remap z [0,1] 
  M[3][2] = -128; // set w = -z 
  M[3][3] = 0; 
}

void setFrustumMatrix(float b, float t, float l, float r, float n, float f, Matrix4x4 M) {
  // set OpenGL perspective projection matrix
  M[0][0] = 2 * n / (r - l); 
  M[1][0] = 0; 
  M[2][0] = 0; 
  M[3][0] = 0; 

  M[0][1] = 0; 
  M[1][1] = 2 * n / (t - b); 
  M[2][1] = 0; 
  M[3][1] = 0; 

  M[0][2] = (r + l) / (r - l); 
  M[1][2] = (t + b) / (t - b); 
  M[2][2] = -(f + n) / (f - n); 
  M[3][2] = -1; 

  M[0][3] = 0; 
  M[1][3] = 0; 
  M[2][3] = -2 * f * n / (f - n); 
  M[3][3] = 0; 
} 