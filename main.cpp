#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>

/* =========================================================================
   GLOBAL VARIABLES & CAMERA STATE (TRUE FPV GAME NAVIGATION SYSTEM)
   ========================================================================= */

// Window display dimensions
int windowWidth = 1000;
int windowHeight = 750;

// Camera Eye position (Player standing position in 3D space)
GLfloat eyeX = 0.0f;
GLfloat eyeY = 6.0f;
GLfloat eyeZ = 28.0f;

// Camera LookAt target position
GLfloat lookX = 0.0f;
GLfloat lookY = 6.0f;
GLfloat lookZ = 0.0f;

// FPV Camera Angles (in radians)
// yaw: horizontal rotation (Left/Right look around - standing in place)
// pitch: vertical elevation (Up/Down tilt look around - standing in place)
GLfloat yaw   = 0.0f;
GLfloat pitch = 0.0f;

// Field of View (Optical Zoom factor for gluPerspective)
GLdouble fov = 45.0;

// Default initial camera positions for resetting (Key '0')
const GLfloat defaultEyeX  = 0.0f;
const GLfloat defaultEyeY  = 6.0f;
const GLfloat defaultEyeZ  = 28.0f;
const GLfloat defaultYaw   = 0.0f;
const GLfloat defaultPitch = 0.0f;
const GLdouble defaultFov  = 45.0;

// Movement speeds
const GLfloat moveSpeed  = 0.8f;  // Walking speed for WASD
const GLfloat angleSpeed = 0.06f; // Turning/Looking speed for Arrow keys (~3.4 degrees)


/* =========================================================================
   CAMERA ORIENTATION HELPER (DIRECTION VECTOR & SIGHT RAY)
   ========================================================================= */

void updateCameraLook()
{
    // Direction vector based on yaw and pitch
    GLfloat dirX = (GLfloat)(cos(pitch) * sin(yaw));
    GLfloat dirY = (GLfloat)(sin(pitch));
    GLfloat dirZ = (GLfloat)(-cos(pitch) * cos(yaw));

    // Target point in space along line of sight
    lookX = eyeX + dirX * 10.0f;
    lookY = eyeY + dirY * 10.0f;
    lookZ = eyeZ + dirZ * 10.0f;
}


/* =========================================================================
   GEOMETRY DATA: UNIT CUBE VERTICES & QUAD INDICES (FROM LAB-02)
   ========================================================================= */

// 8 vertices for a 1x1x1 unit cube in object coordinate space [0, 1]
static GLfloat v_cube[8][3] = {
    {0.0f, 0.0f, 0.0f}, // index 0: bottom-back-left
    {1.0f, 0.0f, 0.0f}, // index 1: bottom-back-right
    {1.0f, 1.0f, 0.0f}, // index 2: top-back-right
    {0.0f, 1.0f, 0.0f}, // index 3: top-back-left
    {0.0f, 0.0f, 1.0f}, // index 4: bottom-front-left
    {1.0f, 0.0f, 1.0f}, // index 5: bottom-front-right
    {1.0f, 1.0f, 1.0f}, // index 6: top-front-right
    {0.0f, 1.0f, 1.0f}  // index 7: top-front-left
};

// 6 quad planes with anti-clockwise vertex ordering for outward normal generation
static GLubyte cube_indices[6][4] = {
    {1, 0, 3, 2}, // Back face   (z = 0, normal points to -Z)
    {4, 5, 6, 7}, // Front face  (z = 1, normal points to +Z)
    {0, 1, 5, 4}, // Bottom face (y = 0, normal points to -Y)
    {7, 6, 2, 3}, // Top face    (y = 1, normal points to +Y)
    {0, 4, 7, 3}, // Left face   (x = 0, normal points to -X)
    {5, 1, 2, 6}  // Right face  (x = 1, normal points to +X)
};


/* =========================================================================
   NORMAL CALCULATION FUNCTION: getNormal3p (FROM KUET LAB-02, PAGE 2)
   ========================================================================= */

static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1,
                        GLfloat x2, GLfloat y2, GLfloat z2,
                        GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    // Vector U = Point2 - Point1
    Ux = x2 - x1;
    Uy = y2 - y1;
    Uz = z2 - z1;

    // Vector V = Point3 - Point1
    Vx = x3 - x1;
    Vy = y3 - y1;
    Vz = z3 - z1;

    // Cross Product: N = U x V
    Nx = Uy * Vz - Uz * Vy;
    Ny = Uz * Vx - Ux * Vz;
    Nz = Ux * Vy - Uy * Vx;

    // Specify surface normal vector
    glNormal3f(Nx, Ny, Nz);
}


/* =========================================================================
   CUSTOM TRANSLATION FUNCTION: ownTranslatef (FROM KUET LAB-02, PAGE 4)
   ========================================================================= */

void ownTranslatef(GLfloat dx, GLfloat dy, GLfloat dz)
{
    GLfloat m[16];
    m[0] = 1.0f; m[4] = 0.0f; m[8]  = 0.0f; m[12] = dx;
    m[1] = 0.0f; m[5] = 1.0f; m[9]  = 0.0f; m[13] = dy;
    m[2] = 0.0f; m[6] = 0.0f; m[10] = 1.0f; m[14] = dz;
    m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;

    glMatrixMode(GL_MODELVIEW);
    glMultMatrixf(m);
}


/* =========================================================================
   CORE 3D PRIMITIVE DRAWING FUNCTIONS
   ========================================================================= */

// Draws a 1x1x1 unit cube using GL_QUADS with anti-clockwise vertices & calculated normals
void drawUnitCube()
{
    glBegin(GL_QUADS);
    for (int i = 0; i < 6; i++)
    {
        getNormal3p(v_cube[cube_indices[i][0]][0], v_cube[cube_indices[i][0]][1], v_cube[cube_indices[i][0]][2],
                    v_cube[cube_indices[i][1]][0], v_cube[cube_indices[i][1]][1], v_cube[cube_indices[i][1]][2],
                    v_cube[cube_indices[i][2]][0], v_cube[cube_indices[i][2]][1], v_cube[cube_indices[i][2]][2]);

        glVertex3fv(&v_cube[cube_indices[i][0]][0]);
        glVertex3fv(&v_cube[cube_indices[i][1]][0]);
        glVertex3fv(&v_cube[cube_indices[i][2]][0]);
        glVertex3fv(&v_cube[cube_indices[i][3]][0]);
    }
    glEnd();
}

// Utility function to draw any colored 3D rectangular box at (x, y, z) with size (sx, sy, sz)
void drawSolidBox(GLfloat x, GLfloat y, GLfloat z,
                  GLfloat sx, GLfloat sy, GLfloat sz,
                  GLfloat r, GLfloat g, GLfloat b)
{
    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(sx, sy, sz);
    drawUnitCube();
    glPopMatrix();
}


/* =========================================================================
   FURNITURE MODULES: BED, SOFA, TABLE, CHAIR, AC, FAN, LIGHT, TV, ETC.
   ========================================================================= */

// 1. Bed with wooden frame, mattress, duvet, pillows and headboard
void drawBed(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat length, GLfloat r, GLfloat g, GLfloat b)
{
    // Wooden bed base
    drawSolidBox(x, y, z, width, 0.35f, length, 0.35f, 0.18f, 0.08f);
    // White comfortable mattress
    drawSolidBox(x + 0.10f, y + 0.35f, z + 0.10f, width - 0.20f, 0.30f, length - 0.20f, 0.95f, 0.95f, 0.95f);
    // Colored bedsheet / duvet blanket
    drawSolidBox(x + 0.10f, y + 0.38f, z + 0.10f, width - 0.20f, 0.30f, length * 0.68f, r, g, b);
    // Wooden headboard
    drawSolidBox(x, y, z + length - 0.15f, width, 1.35f, 0.15f, 0.28f, 0.14f, 0.06f);
    // Two soft pillows
    GLfloat pWidth = (width - 0.40f) * 0.50f;
    drawSolidBox(x + 0.15f, y + 0.65f, z + length - 0.85f, pWidth, 0.14f, 0.60f, 1.0f, 1.0f, 1.0f);
    drawSolidBox(x + 0.25f + pWidth, y + 0.65f, z + length - 0.85f, pWidth, 0.14f, 0.60f, 1.0f, 1.0f, 1.0f);
}

// 2. Sofa with base, seat cushions, backrest, armrests and throw pillows
void drawSofa(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat depth, GLfloat r, GLfloat g, GLfloat b)
{
    // Base platform
    drawSolidBox(x, y, z, width, 0.20f, depth, 0.20f, 0.20f, 0.22f);
    // Main seat cushion
    drawSolidBox(x + 0.12f, y + 0.20f, z + 0.10f, width - 0.24f, 0.35f, depth - 0.15f, r, g, b);
    // Backrest
    drawSolidBox(x, y + 0.20f, z, width, 0.75f, 0.25f, r * 0.90f, g * 0.90f, b * 0.90f);
    // Left armrest
    drawSolidBox(x, y + 0.20f, z, 0.22f, 0.55f, depth, r * 0.82f, g * 0.82f, b * 0.82f);
    // Right armrest
    drawSolidBox(x + width - 0.22f, y + 0.20f, z, 0.22f, 0.55f, depth, r * 0.82f, g * 0.82f, b * 0.82f);
    // Throw accent cushions
    drawSolidBox(x + 0.30f, y + 0.55f, z + 0.22f, 0.45f, 0.35f, 0.12f, 0.95f, 0.80f, 0.25f);
    drawSolidBox(x + width - 0.75f, y + 0.55f, z + 0.22f, 0.45f, 0.35f, 0.12f, 0.95f, 0.80f, 0.25f);
}

// 3. Single Armchair
void drawSingleChair(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat d, GLfloat r, GLfloat g, GLfloat b)
{
    drawSolidBox(x, y, z, w, 0.18f, d, 0.20f, 0.20f, 0.20f);
    drawSolidBox(x + 0.08f, y + 0.18f, z + 0.08f, w - 0.16f, 0.32f, d - 0.12f, r, g, b);
    drawSolidBox(x, y + 0.18f, z, w, 0.65f, 0.20f, r * 0.88f, g * 0.88f, b * 0.88f);
    drawSolidBox(x, y + 0.18f, z, 0.16f, 0.48f, d, r * 0.80f, g * 0.80f, b * 0.80f);
    drawSolidBox(x + w - 0.16f, y + 0.18f, z, 0.16f, 0.48f, d, r * 0.80f, g * 0.80f, b * 0.80f);
}

// 4. Coffee Table
void drawCoffeeTable(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat d, GLfloat r, GLfloat g, GLfloat b)
{
    // 4 legs
    drawSolidBox(x + 0.05f, y, z + 0.05f, 0.08f, 0.45f, 0.08f, 0.22f, 0.22f, 0.25f);
    drawSolidBox(x + w - 0.13f, y, z + 0.05f, 0.08f, 0.45f, 0.08f, 0.22f, 0.22f, 0.25f);
    drawSolidBox(x + 0.05f, y, z + d - 0.13f, 0.08f, 0.45f, 0.08f, 0.22f, 0.22f, 0.25f);
    drawSolidBox(x + w - 0.13f, y, z + d - 0.13f, 0.08f, 0.45f, 0.08f, 0.22f, 0.22f, 0.25f);
    // Tabletop surface
    drawSolidBox(x, y + 0.45f, z, w, 0.08f, d, r, g, b);
    // Center magazine / decor piece
    drawSolidBox(x + w * 0.35f, y + 0.53f, z + d * 0.30f, w * 0.30f, 0.03f, d * 0.40f, 0.90f, 0.30f, 0.20f);
}

// 5. Dining / Meeting Table and 4 Chairs
void drawTableAndChairs(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat d)
{
    // Table legs
    drawSolidBox(x + 0.10f, y, z + 0.10f, 0.10f, 0.90f, 0.10f, 0.30f, 0.15f, 0.08f);
    drawSolidBox(x + w - 0.20f, y, z + 0.10f, 0.10f, 0.90f, 0.10f, 0.30f, 0.15f, 0.08f);
    drawSolidBox(x + 0.10f, y, z + d - 0.20f, 0.10f, 0.90f, 0.10f, 0.30f, 0.15f, 0.08f);
    drawSolidBox(x + w - 0.20f, y, z + d - 0.20f, 0.10f, 0.90f, 0.10f, 0.30f, 0.15f, 0.08f);
    // Tabletop
    drawSolidBox(x, y + 0.90f, z, w, 0.10f, d, 0.48f, 0.26f, 0.14f);

    // 2 Chairs on side 1
    for (int c = 0; c < 2; c++)
    {
        GLfloat cx = x + 0.25f + (GLfloat)c * (w * 0.45f);
        drawSolidBox(cx, y + 0.50f, z - 0.45f, 0.45f, 0.06f, 0.40f, 0.35f, 0.18f, 0.10f);
        drawSolidBox(cx, y + 0.50f, z - 0.50f, 0.45f, 0.55f, 0.06f, 0.30f, 0.15f, 0.08f);
        drawSolidBox(cx, y, z - 0.45f, 0.06f, 0.50f, 0.06f, 0.25f, 0.12f, 0.06f);
        drawSolidBox(cx + 0.39f, y, z - 0.45f, 0.06f, 0.50f, 0.06f, 0.25f, 0.12f, 0.06f);
    }
    // 2 Chairs on side 2
    for (int c = 0; c < 2; c++)
    {
        GLfloat cx = x + 0.25f + (GLfloat)c * (w * 0.45f);
        drawSolidBox(cx, y + 0.50f, z + d + 0.05f, 0.45f, 0.06f, 0.40f, 0.35f, 0.18f, 0.10f);
        drawSolidBox(cx, y + 0.50f, z + d + 0.40f, 0.45f, 0.55f, 0.06f, 0.30f, 0.15f, 0.08f);
        drawSolidBox(cx, y, z + d + 0.39f, 0.06f, 0.50f, 0.06f, 0.25f, 0.12f, 0.06f);
        drawSolidBox(cx + 0.39f, y, z + d + 0.39f, 0.06f, 0.50f, 0.06f, 0.25f, 0.12f, 0.06f);
    }
}

// 6. Wall-Mounted Split Air Conditioner (AC)
void drawAC(GLfloat x, GLfloat y, GLfloat z, GLfloat width, bool isSideWall)
{
    if (!isSideWall)
    {
        // AC on Back/Front wall
        drawSolidBox(x, y, z, width, 0.50f, 0.32f, 0.96f, 0.96f, 0.98f);
        drawSolidBox(x + 0.10f, y + 0.04f, z + 0.30f, width - 0.20f, 0.08f, 0.03f, 0.30f, 0.30f, 0.35f);
        drawSolidBox(x + width * 0.72f, y + 0.24f, z + 0.31f, 0.16f, 0.08f, 0.02f, 0.20f, 0.85f, 0.25f);
    }
    else
    {
        // AC on Left/Right wall
        drawSolidBox(x, y, z, 0.32f, 0.50f, width, 0.96f, 0.96f, 0.98f);
        drawSolidBox(x + 0.30f, y + 0.04f, z + 0.10f, 0.03f, 0.08f, width - 0.20f, 0.30f, 0.30f, 0.35f);
        drawSolidBox(x + 0.31f, y + 0.24f, z + width * 0.72f, 0.02f, 0.08f, 0.16f, 0.20f, 0.85f, 0.25f);
    }
}

// 7. Ceiling Fan with hanging downrod, central motor & 4 blades
void drawCeilingFan(GLfloat x, GLfloat y, GLfloat z)
{
    drawSolidBox(x - 0.03f, y - 0.45f, z - 0.03f, 0.06f, 0.45f, 0.06f, 0.25f, 0.25f, 0.28f);
    drawSolidBox(x - 0.22f, y - 0.60f, z - 0.22f, 0.44f, 0.15f, 0.44f, 0.85f, 0.75f, 0.30f);
    // 4 Fan blades
    drawSolidBox(x + 0.22f, y - 0.56f, z - 0.10f, 1.15f, 0.02f, 0.20f, 0.38f, 0.20f, 0.10f);
    drawSolidBox(x - 1.37f, y - 0.56f, z - 0.10f, 1.15f, 0.02f, 0.20f, 0.38f, 0.20f, 0.10f);
    drawSolidBox(x - 0.10f, y - 0.56f, z + 0.22f, 0.20f, 0.02f, 1.15f, 0.38f, 0.20f, 0.10f);
    drawSolidBox(x - 0.10f, y - 0.56f, z - 1.37f, 0.20f, 0.02f, 1.15f, 0.38f, 0.20f, 0.10f);
}

// 8. Ceiling Light / Luminous LED panel lamp
void drawCeilingLight(GLfloat x, GLfloat y, GLfloat z)
{
    drawSolidBox(x - 0.40f, y - 0.04f, z - 0.40f, 0.80f, 0.04f, 0.80f, 0.75f, 0.75f, 0.78f);
    drawSolidBox(x - 0.34f, y - 0.08f, z - 0.34f, 0.68f, 0.04f, 0.68f, 1.00f, 0.98f, 0.65f);
}

// 9. Study Desk with Laptop and Office Chair
void drawStudyDesk(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat d)
{
    drawSolidBox(x + 0.08f, y, z + 0.08f, 0.08f, 0.85f, 0.08f, 0.25f, 0.15f, 0.08f);
    drawSolidBox(x + w - 0.16f, y, z + 0.08f, 0.08f, 0.85f, 0.08f, 0.25f, 0.15f, 0.08f);
    drawSolidBox(x + 0.08f, y, z + d - 0.16f, 0.08f, 0.85f, 0.08f, 0.25f, 0.15f, 0.08f);
    drawSolidBox(x + w - 0.16f, y, z + d - 0.16f, 0.08f, 0.85f, 0.08f, 0.25f, 0.15f, 0.08f);
    drawSolidBox(x, y + 0.85f, z, w, 0.08f, d, 0.42f, 0.22f, 0.12f);

    // Laptop on desk
    drawSolidBox(x + w * 0.35f, y + 0.93f, z + d * 0.25f, 0.45f, 0.02f, 0.35f, 0.22f, 0.22f, 0.25f);
    drawSolidBox(x + w * 0.35f, y + 0.95f, z + d * 0.25f, 0.45f, 0.30f, 0.02f, 0.20f, 0.55f, 0.85f);

    // Chair
    drawSolidBox(x + w * 0.32f, y + 0.45f, z + d + 0.15f, 0.48f, 0.06f, 0.45f, 0.20f, 0.20f, 0.25f);
    drawSolidBox(x + w * 0.32f, y + 0.45f, z + d + 0.55f, 0.48f, 0.55f, 0.06f, 0.20f, 0.20f, 0.25f);
    drawSolidBox(x + w * 0.32f, y, z + d + 0.15f, 0.06f, 0.45f, 0.06f, 0.30f, 0.30f, 0.35f);
    drawSolidBox(x + w * 0.32f + 0.42f, y, z + d + 0.15f, 0.06f, 0.45f, 0.06f, 0.30f, 0.30f, 0.35f);
}

// 10. Large Wall Bookshelf with books
void drawBookshelf(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h, GLfloat d)
{
    drawSolidBox(x, y, z, w, h, d, 0.35f, 0.18f, 0.08f);
    for (int s = 0; s < 3; s++)
    {
        GLfloat sy = y + 0.20f + (GLfloat)s * (h * 0.28f);
        drawSolidBox(x + 0.10f, sy + 0.55f, z + 0.05f, w - 0.20f, 0.05f, d - 0.05f, 0.45f, 0.25f, 0.12f);
        drawSolidBox(x + 0.20f, sy, z + 0.10f, (w - 0.40f) * 0.30f, 0.50f, d - 0.15f, 0.85f, 0.20f, 0.20f);
        drawSolidBox(x + 0.20f + (w - 0.40f) * 0.32f, sy, z + 0.10f, (w - 0.40f) * 0.30f, 0.48f, d - 0.15f, 0.20f, 0.50f, 0.85f);
        drawSolidBox(x + 0.20f + (w - 0.40f) * 0.64f, sy, z + 0.10f, (w - 0.40f) * 0.28f, 0.52f, d - 0.15f, 0.20f, 0.75f, 0.35f);
    }
}

// 11. Wardrobe / Closet
void drawWardrobe(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h, GLfloat d)
{
    drawSolidBox(x, y, z, w, h, d, 0.38f, 0.20f, 0.10f);
    drawSolidBox(x + w * 0.50f - 0.01f, y + 0.10f, z + d, 0.02f, h - 0.20f, 0.02f, 0.20f, 0.10f, 0.05f);
    drawSolidBox(x + w * 0.50f - 0.06f, y + h * 0.50f, z + d + 0.02f, 0.03f, 0.20f, 0.03f, 0.90f, 0.80f, 0.25f);
    drawSolidBox(x + w * 0.50f + 0.03f, y + h * 0.50f, z + d + 0.02f, 0.03f, 0.20f, 0.03f, 0.90f, 0.80f, 0.25f);
}

// 12. TV and Entertainment Media Console
void drawEntertainmentUnit(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h)
{
    drawSolidBox(x, y, z, w, 0.60f, 0.70f, 0.25f, 0.14f, 0.06f);
    drawSolidBox(x + w * 0.40f, y + 0.60f, z + 0.25f, w * 0.20f, 0.15f, 0.20f, 0.20f, 0.20f, 0.22f);
    drawSolidBox(x + 0.20f, y + 0.75f, z + 0.28f, w - 0.40f, h - 0.75f, 0.08f, 0.12f, 0.12f, 0.14f);
    drawSolidBox(x + 0.25f, y + 0.80f, z + 0.35f, w - 0.50f, h - 0.85f, 0.02f, 0.18f, 0.65f, 0.95f);
}


/* =========================================================================
   ROOM INTERIOR FURNISHING BY FLOOR
   ========================================================================= */

void drawFloorFurniture(int floor, GLfloat floorY, GLfloat ceilY)
{
    if (floor == 0)
    {
        // -----------------------------------------------------------------
        // GROUND FLOOR: LIVING ROOM & RECEPTION LOUNGE
        // -----------------------------------------------------------------
        drawSofa(-4.80f, floorY, -0.60f, 3.40f, 1.40f, 0.22f, 0.42f, 0.72f);
        drawCoffeeTable(-4.30f, floorY, 1.30f, 2.20f, 1.00f, 0.48f, 0.26f, 0.14f);
        drawSingleChair(-1.10f, floorY, -0.40f, 1.20f, 1.20f, 0.30f, 0.50f, 0.80f);
        drawEntertainmentUnit(-4.60f, floorY, -3.75f, 2.80f, 2.00f);
        drawTableAndChairs(1.80f, floorY, -1.80f, 2.40f, 1.40f);
        drawAC(-5.75f, floorY + 2.05f, -1.20f, 1.80f, true);
        drawCeilingFan(-3.20f, ceilY, 0.50f);
        drawCeilingFan(3.00f, ceilY, -1.10f);
        drawCeilingLight(-3.20f, ceilY, 0.50f);
        drawCeilingLight(3.00f, ceilY, -1.10f);
    }
    else if (floor == 1)
    {
        // -----------------------------------------------------------------
        // 1ST FLOOR: MASTER BEDROOM SUITE
        // -----------------------------------------------------------------
        drawBed(-1.50f, floorY, -3.65f, 3.00f, 3.30f, 0.82f, 0.22f, 0.24f);
        drawWardrobe(-5.75f, floorY, -2.40f, 0.75f, 2.30f, 2.20f);
        drawStudyDesk(2.60f, floorY, -1.20f, 1.80f, 1.00f);
        drawSingleChair(-4.60f, floorY, 1.50f, 1.10f, 1.10f, 0.70f, 0.45f, 0.25f);
        drawAC(-5.75f, floorY + 2.05f, 0.60f, 1.80f, true);
        drawCeilingFan(0.00f, ceilY, -0.80f);
        drawCeilingLight(0.00f, ceilY, -0.80f);
    }
    else if (floor == 2)
    {
        // -----------------------------------------------------------------
        // 2ND FLOOR: EXECUTIVE OFFICE & STUDY LIBRARY
        // -----------------------------------------------------------------
        drawStudyDesk(-3.60f, floorY, -1.20f, 2.40f, 1.20f);
        drawBookshelf(-0.80f, floorY, -3.75f, 3.20f, 2.30f, 0.55f);
        drawSofa(2.40f, floorY, -0.80f, 2.60f, 1.20f, 0.22f, 0.52f, 0.35f);
        drawCoffeeTable(2.60f, floorY, 0.80f, 1.80f, 0.80f, 0.40f, 0.25f, 0.15f);
        drawAC(5.55f, floorY + 2.05f, -1.50f, 1.80f, true);
        drawCeilingFan(-1.20f, ceilY, 0.00f);
        drawCeilingLight(-1.20f, ceilY, 0.00f);
        drawCeilingLight(3.00f, ceilY, 0.00f);
    }
    else if (floor == 3)
    {
        // -----------------------------------------------------------------
        // 3RD FLOOR: GUEST BEDROOM & STUDIO
        // -----------------------------------------------------------------
        drawBed(-4.80f, floorY, -3.65f, 2.60f, 3.10f, 0.20f, 0.55f, 0.75f);
        drawWardrobe(-1.60f, floorY, -3.75f, 1.80f, 2.30f, 0.65f);
        drawStudyDesk(2.60f, floorY, -1.20f, 1.80f, 1.00f);
        drawSofa(1.50f, floorY, 1.20f, 2.40f, 1.10f, 0.72f, 0.42f, 0.20f);
        drawAC(-5.75f, floorY + 2.05f, -0.60f, 1.80f, true);
        drawCeilingFan(-1.80f, ceilY, -0.60f);
        drawCeilingLight(-1.80f, ceilY, -0.60f);
    }
    else if (floor == 4)
    {
        // -----------------------------------------------------------------
        // 4TH FLOOR: PENTHOUSE ENTERTAINMENT SUITE
        // -----------------------------------------------------------------
        drawSofa(-4.80f, floorY, -1.00f, 3.60f, 1.40f, 0.85f, 0.82f, 0.80f);
        drawCoffeeTable(-4.20f, floorY, 0.90f, 2.20f, 1.10f, 0.45f, 0.75f, 0.95f);
        drawEntertainmentUnit(1.80f, floorY, -3.75f, 3.20f, 2.20f);
        drawTableAndChairs(2.00f, floorY, 0.60f, 2.20f, 1.20f);
        drawAC(-5.75f, floorY + 2.10f, 0.80f, 2.00f, true);
        drawCeilingFan(-1.00f, ceilY, 0.30f);
        drawCeilingLight(-1.00f, ceilY, 0.30f);
    }
}


/* =========================================================================
   ARCHITECTURAL COMPONENT: GLASS WINDOW (JANALA)
   ========================================================================= */

void drawGlassWindow(GLfloat x, GLfloat y, GLfloat z,
                     GLfloat width, GLfloat height,
                     bool isSideFacing)
{
    if (!isSideFacing)
    {
        // Front/Back facing window (aligns with XY plane)
        // 1. Dark charcoal window frame
        drawSolidBox(x, y, z, width, height, 0.10f, 0.22f, 0.22f, 0.25f);

        // 2. Clear sky-blue reflective glass pane
        drawSolidBox(x + 0.10f, y + 0.10f, z + 0.02f,
                     width - 0.20f, height - 0.20f, 0.06f,
                     0.40f, 0.75f, 0.95f);

        // 3. Glass reflection highlight (gives realistic shiny look)
        drawSolidBox(x + 0.15f, y + 0.15f, z + 0.05f,
                     (width - 0.30f) * 0.30f, height - 0.30f, 0.04f,
                     0.75f, 0.90f, 1.00f);

        // 4. White mullions / window cross-grills
        // Vertical mullion
        drawSolidBox(x + width * 0.50f - 0.03f, y + 0.10f, z + 0.05f,
                     0.06f, height - 0.20f, 0.05f,
                     0.90f, 0.90f, 0.95f);
        // Horizontal mullion
        drawSolidBox(x + 0.10f, y + height * 0.50f - 0.03f, z + 0.05f,
                     width - 0.20f, 0.06f, 0.05f,
                     0.90f, 0.90f, 0.95f);

        // 5. Sunshade / Lintel (Chhajja) above the window
        drawSolidBox(x - 0.15f, y + height, z - 0.05f,
                     width + 0.30f, 0.10f, 0.45f,
                     0.85f, 0.85f, 0.88f);
    }
    else
    {
        // Side facing window (aligns with YZ plane)
        // 1. Dark frame
        drawSolidBox(x, y, z, 0.10f, height, width, 0.22f, 0.22f, 0.25f);

        // 2. Glass pane
        drawSolidBox(x + 0.02f, y + 0.10f, z + 0.10f,
                     0.06f, height - 0.20f, width - 0.20f,
                     0.40f, 0.75f, 0.95f);

        // 3. Glass highlight
        drawSolidBox(x + 0.05f, y + 0.15f, z + 0.15f,
                     0.04f, height - 0.30f, (width - 0.30f) * 0.30f,
                     0.75f, 0.90f, 1.00f);

        // 4. Cross mullions
        drawSolidBox(x + 0.05f, y + 0.10f, z + width * 0.50f - 0.03f,
                     0.05f, height - 0.20f, 0.06f,
                     0.90f, 0.90f, 0.95f);
        drawSolidBox(x + 0.05f, y + height * 0.50f - 0.03f, z + 0.10f,
                     0.05f, 0.06f, width - 0.20f,
                     0.90f, 0.90f, 0.95f);

        // 5. Sunshade above window
        drawSolidBox(x - 0.35f, y + height, z - 0.15f,
                     0.45f, 0.10f, width + 0.30f,
                     0.85f, 0.85f, 0.88f);
    }
}


/* =========================================================================
   ARCHITECTURAL COMPONENT: GROUND FLOOR MAIN DOOR (MAIN DOR)
   ========================================================================= */

void drawMainDoor(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height)
{
    // 1. Entrance stairs leading to door
    drawSolidBox(x - 0.8f, 0.00f, z + 0.80f, width + 1.6f, 0.14f, 0.40f, 0.60f, 0.60f, 0.65f);
    drawSolidBox(x - 0.5f, 0.14f, z + 0.40f, width + 1.0f, 0.13f, 0.40f, 0.68f, 0.68f, 0.72f);
    drawSolidBox(x - 0.2f, 0.27f, z + 0.00f, width + 0.4f, 0.13f, 0.40f, 0.76f, 0.76f, 0.80f);

    // 2. Heavy dark-wood outer door frame
    drawSolidBox(x, y, z, width, height, 0.18f, 0.25f, 0.12f, 0.05f);

    // 3. Double-leaf wooden doors (Teak / Mahogany brown)
    GLfloat panelWidth = (width - 0.24f) * 0.50f;
    // Left door leaf
    drawSolidBox(x + 0.08f, y + 0.06f, z + 0.03f,
                 panelWidth, height - 0.12f, 0.12f,
                 0.48f, 0.24f, 0.12f);
    // Right door leaf
    drawSolidBox(x + 0.08f + panelWidth + 0.08f, y + 0.06f, z + 0.03f,
                 panelWidth, height - 0.12f, 0.12f,
                 0.48f, 0.24f, 0.12f);

    // 4. Carved decorative panel insets
    for (int p = 0; p < 2; p++)
    {
        GLfloat px = (p == 0) ? (x + 0.14f) : (x + 0.08f + panelWidth + 0.14f);
        drawSolidBox(px, y + 0.20f, z + 0.10f,
                     panelWidth - 0.12f, (height - 0.12f) * 0.38f, 0.06f,
                     0.35f, 0.16f, 0.08f);
        drawSolidBox(px, y + (height - 0.12f) * 0.52f, z + 0.10f,
                     panelWidth - 0.12f, (height - 0.12f) * 0.38f, 0.06f,
                     0.35f, 0.16f, 0.08f);
    }

    // 5. Golden brass door handles
    GLfloat handleY = y + height * 0.46f;
    drawSolidBox(x + 0.08f + panelWidth - 0.12f, handleY, z + 0.16f,
                 0.05f, 0.32f, 0.05f,
                 0.95f, 0.82f, 0.22f);
    drawSolidBox(x + 0.08f + panelWidth + 0.08f + 0.07f, handleY, z + 0.16f,
                 0.05f, 0.32f, 0.05f,
                 0.95f, 0.82f, 0.22f);

    // 6. Porch entrance canopy above the main door
    drawSolidBox(x - 0.6f, y + height, z - 0.10f,
                 width + 1.2f, 0.22f, 1.80f,
                 0.86f, 0.86f, 0.90f);
}


/* =========================================================================
   ARCHITECTURAL COMPONENT: DEDICATED BALCONY WITH DOOR (NO WINDOW IN BALCONY)
   ========================================================================= */

void drawBalcony(GLfloat baseY)
{
    // The balcony is cleanly positioned on the RIGHT side of the facade (X: 1.0 to 5.0)
    // Completely separate from the left-side glass window!
    GLfloat bx = 1.00f;
    GLfloat bw = 4.00f;
    GLfloat bz = 4.00f;
    GLfloat bd = 1.35f;

    // 1. Projecting balcony concrete floor slab
    drawSolidBox(bx, baseY, bz, bw, 0.25f, bd, 0.88f, 0.88f, 0.90f);

    // 2. Modern glass railing panels
    // Front glass panel
    drawSolidBox(bx + 0.05f, baseY + 0.25f, bz + bd - 0.05f, bw - 0.10f, 0.85f, 0.05f, 0.45f, 0.78f, 0.95f);
    // Left side glass panel
    drawSolidBox(bx, baseY + 0.25f, bz, 0.05f, 0.85f, bd - 0.05f, 0.45f, 0.78f, 0.95f);
    // Right side glass panel
    drawSolidBox(bx + bw - 0.05f, baseY + 0.25f, bz, 0.05f, 0.85f, bd - 0.05f, 0.45f, 0.78f, 0.95f);

    // 3. Polished stainless steel handrails
    // Front handrail
    drawSolidBox(bx, baseY + 1.10f, bz + bd - 0.08f, bw, 0.08f, 0.08f, 0.30f, 0.30f, 0.35f);
    // Left handrail
    drawSolidBox(bx, baseY + 1.10f, bz, 0.08f, 0.08f, bd, 0.30f, 0.30f, 0.35f);
    // Right handrail
    drawSolidBox(bx + bw - 0.08f, baseY + 1.10f, bz, 0.08f, 0.08f, bd, 0.30f, 0.30f, 0.35f);

    // 4. Balcony entrance door on the room wall (Door leading onto balcony, NOT a window!)
    // Outer frame
    drawSolidBox(bx + 1.00f, baseY + 0.25f, bz - 0.02f, 2.00f, 2.30f, 0.08f, 0.25f, 0.14f, 0.08f);
    // Double sliding balcony doors (Tinted glass panels with frames)
    drawSolidBox(bx + 1.08f, baseY + 0.33f, bz + 0.01f, 0.88f, 2.12f, 0.04f, 0.40f, 0.75f, 0.95f);
    drawSolidBox(bx + 2.04f, baseY + 0.33f, bz + 0.01f, 0.88f, 2.12f, 0.04f, 0.40f, 0.75f, 0.95f);
    // Brass door handles
    drawSolidBox(bx + 1.90f, baseY + 1.20f, bz + 0.05f, 0.04f, 0.25f, 0.04f, 0.90f, 0.80f, 0.25f);
    drawSolidBox(bx + 2.06f, baseY + 1.20f, bz + 0.05f, 0.04f, 0.25f, 0.04f, 0.90f, 0.80f, 0.25f);
}


/* =========================================================================
   ARCHITECTURAL COMPONENT: MULTI-TIER STAIRCASE (SIRI SYSTEM)
   CONNECTS GROUND FLOOR -> 1ST -> 2ND -> 3RD -> 4TH -> ROOFTOP
   ========================================================================= */

void drawStaircaseSystem()
{
    GLfloat stairX = 6.00f; // Attached to the right side of the building
    GLfloat stairWidth = 2.00f;
    int stepsPerFlight = 6;
    GLfloat stepHeight = 3.00f / (GLfloat)stepsPerFlight; // 0.50f per step
    GLfloat stepDepth  = 0.60f;

    for (int floor = 0; floor < 5; floor++)
    {
        GLfloat startY = 0.40f + (GLfloat)floor * 3.00f;

        // 1. Floor landing platform connected to the floor door
        drawSolidBox(stairX, startY, -2.50f, stairWidth + 0.40f, 0.25f, 1.80f, 0.75f, 0.75f, 0.78f);

        // Landing safety handrail
        drawSolidBox(stairX + stairWidth + 0.35f, startY + 0.25f, -2.50f,
                     0.06f, 0.90f, 1.80f,
                     0.30f, 0.30f, 0.35f);

        // Access door leading into the floor interior from stair landing
        drawSolidBox(stairX - 0.05f, startY + 0.25f, -2.10f,
                     0.06f, 2.20f, 1.10f,
                     0.35f, 0.18f, 0.08f);

        // 2. Flight of steps climbing from startY to startY + 3.0f
        for (int s = 0; s < stepsPerFlight; s++)
        {
            GLfloat currentY = startY + (GLfloat)s * stepHeight;
            GLfloat currentZ = -0.70f + (GLfloat)s * stepDepth;

            // Marble step block (Riser + Tread)
            drawSolidBox(stairX, currentY, currentZ,
                         stairWidth, stepHeight, stepDepth,
                         0.82f, 0.82f, 0.85f);

            // Dark tread bullnose edge
            drawSolidBox(stairX, currentY + stepHeight - 0.03f, currentZ,
                         stairWidth, 0.03f, stepDepth,
                         0.30f, 0.30f, 0.35f);

            // Vertical baluster / railing post
            drawSolidBox(stairX + stairWidth - 0.10f, currentY + stepHeight, currentZ + stepDepth * 0.50f - 0.03f,
                         0.06f, 0.85f, 0.06f,
                         0.35f, 0.35f, 0.38f);
        }

        // 3. Continuous inclined handrail alongside the steps
        glPushMatrix();
        glTranslatef(stairX + stairWidth - 0.10f, startY + stepHeight + 0.85f, -0.70f);
        glRotatef(39.8f, -1.0f, 0.0f, 0.0f);
        drawSolidBox(0.0f, 0.0f, 0.0f, 0.08f, 0.08f, 4.60f, 0.25f, 0.25f, 0.28f);
        glPopMatrix();

        // 4. Upper landing platform connecting to next floor level
        drawSolidBox(stairX, startY + 3.00f, 2.90f, stairWidth + 0.40f, 0.25f, 1.80f, 0.75f, 0.75f, 0.78f);
        drawSolidBox(stairX + stairWidth + 0.35f, startY + 3.25f, 2.90f,
                     0.06f, 0.90f, 1.80f,
                     0.30f, 0.30f, 0.35f);
    }

    // Heavy vertical support pillars for staircase structure
    drawSolidBox(stairX + stairWidth + 0.30f, 0.0f, -2.50f, 0.25f, 15.65f, 0.25f, 0.70f, 0.70f, 0.75f);
    drawSolidBox(stairX + stairWidth + 0.30f, 0.0f,  4.45f, 0.25f, 15.65f, 0.25f, 0.70f, 0.70f, 0.75f);
}


/* =========================================================================
   ARCHITECTURAL COMPONENT: ROOFTOP (ROOF SLAB, PARAPET, MUMTY, WATER TANK)
   ========================================================================= */

void drawRooftop()
{
    // 1. Rooftop concrete slab
    drawSolidBox(-6.40f, 15.40f, -4.40f, 12.80f, 0.30f, 8.80f, 0.88f, 0.88f, 0.90f);

    // 2. Terrace floor tiles surface
    drawSolidBox(-6.20f, 15.70f, -4.20f, 12.40f, 0.02f, 8.40f, 0.75f, 0.70f, 0.64f);

    // 3. Parapet safety boundary wall (1.0m height around roof edges)
    drawSolidBox(-6.30f, 15.70f,  4.15f, 12.60f, 0.95f, 0.25f, 0.90f, 0.88f, 0.84f);
    drawSolidBox(-6.30f, 15.70f, -4.40f, 12.60f, 0.95f, 0.25f, 0.90f, 0.88f, 0.84f);
    drawSolidBox(-6.40f, 15.70f, -4.40f,  0.25f, 0.95f, 8.80f, 0.90f, 0.88f, 0.84f);
    drawSolidBox( 6.15f, 15.70f,  2.00f,  0.25f, 0.95f, 2.40f, 0.90f, 0.88f, 0.84f);

    // Parapet coping caps (dark top protective trim)
    drawSolidBox(-6.40f, 16.65f,  4.10f, 12.80f, 0.08f, 0.35f, 0.45f, 0.45f, 0.48f);
    drawSolidBox(-6.40f, 16.65f, -4.45f, 12.80f, 0.08f, 0.35f, 0.45f, 0.45f, 0.48f);
    drawSolidBox(-6.45f, 16.65f, -4.45f,  0.35f, 0.08f, 8.90f, 0.45f, 0.45f, 0.48f);

    // 4. Rooftop Mumty Room (Stairhead exit enclosure room)
    drawSolidBox(3.60f, 15.70f, -3.20f, 4.80f, 2.80f, 5.00f, 0.90f, 0.88f, 0.84f);
    drawSolidBox(3.40f, 18.50f, -3.40f, 5.20f, 0.25f, 5.40f, 0.85f, 0.85f, 0.88f);
    drawSolidBox(3.55f, 15.72f, -1.00f, 0.10f, 2.30f, 1.20f, 0.45f, 0.22f, 0.10f);
    drawSolidBox(3.50f, 16.80f, -0.15f, 0.08f, 0.25f, 0.05f, 0.95f, 0.82f, 0.22f);

    // 5. Overhead Water Reservoir / Water Tank
    drawSolidBox(4.40f, 18.75f, -2.40f, 0.30f, 0.80f, 0.30f, 0.65f, 0.65f, 0.68f);
    drawSolidBox(7.20f, 18.75f, -2.40f, 0.30f, 0.80f, 0.30f, 0.65f, 0.65f, 0.68f);
    drawSolidBox(4.40f, 18.75f,  0.60f, 0.30f, 0.80f, 0.30f, 0.65f, 0.65f, 0.68f);
    drawSolidBox(7.20f, 18.75f,  0.60f, 0.30f, 0.80f, 0.30f, 0.65f, 0.65f, 0.68f);
    drawSolidBox(4.20f, 19.55f, -2.60f, 3.50f, 0.15f, 3.70f, 0.55f, 0.55f, 0.58f);
    drawSolidBox(4.40f, 19.70f, -2.40f, 3.10f, 1.80f, 3.30f, 0.18f, 0.52f, 0.82f);
    drawSolidBox(5.00f, 21.50f, -1.80f, 1.90f, 0.18f, 2.10f, 0.12f, 0.38f, 0.65f);
    drawSolidBox(7.30f, 15.70f, -1.00f, 0.10f, 4.40f, 0.10f, 0.40f, 0.40f, 0.45f);

    // 6. Rooftop Lightning Arrester / Antenna Mast
    drawSolidBox(5.90f, 21.68f, -0.80f, 0.08f, 2.40f, 0.08f, 0.85f, 0.85f, 0.90f);
}


/* =========================================================================
   ARCHITECTURAL COMPONENT: GROUND ENVIRONMENT & LAWN
   ========================================================================= */

void drawGroundAndEnvironment()
{
    // Wide green lawn base
    drawSolidBox(-22.0f, -0.20f, -18.0f, 44.0f, 0.20f, 36.0f, 0.28f, 0.65f, 0.28f);
    // Stone paved pathway leading up to main entrance door
    drawSolidBox(-2.50f, -0.05f, 4.00f, 5.00f, 0.08f, 14.00f, 0.65f, 0.65f, 0.68f);
    // Building plinth foundation base
    drawSolidBox(-6.60f, 0.00f, -4.60f, 15.20f, 0.40f, 9.20f, 0.45f, 0.45f, 0.48f);
}


/* =========================================================================
   SCENE ASSEMBLY: 4-FLOOR BUILDING WITH HOLLOW ROOMS & FULL FURNITURE
   ========================================================================= */

void drawScene()
{
    // Draw ground lawn and foundation
    drawGroundAndEnvironment();

    // ---------------------------------------------------------------------
    // GROUND FLOOR (Floor 0: Y = 0.4 to 3.4)
    // ---------------------------------------------------------------------
    GLfloat gFloorY = 0.40f;
    GLfloat gCeilY  = 3.25f;

    // Floor tile slab
    drawSolidBox(-6.0f, gFloorY, -4.0f, 12.0f, 0.15f, 8.0f, 0.86f, 0.84f, 0.80f);
    // Ceiling slab
    drawSolidBox(-6.0f, gCeilY, -4.0f, 12.0f, 0.15f, 8.0f, 0.92f, 0.92f, 0.92f);

    // Back wall
    drawSolidBox(-6.0f, gFloorY, -4.0f, 12.0f, 2.85f, 0.20f, 0.92f, 0.90f, 0.85f);
    // Left side wall
    drawSolidBox(-6.0f, gFloorY, -4.0f, 0.20f, 2.85f, 8.0f, 0.92f, 0.90f, 0.85f);
    // Right side wall
    drawSolidBox(5.80f, gFloorY, -4.0f, 0.20f, 2.85f, 8.0f, 0.92f, 0.90f, 0.85f);

    // Front wall: Left wing & Right wing (leaving entrance opening at center)
    drawSolidBox(-6.0f, gFloorY, 3.80f, 4.50f, 2.85f, 0.20f, 0.92f, 0.90f, 0.85f);
    drawSolidBox( 1.50f, gFloorY, 3.80f, 4.50f, 2.85f, 0.20f, 0.92f, 0.90f, 0.85f);

    // Main entrance door on Ground Floor
    drawMainDoor(-1.50f, gFloorY, 4.02f, 3.00f, 2.40f);

    // Ground floor front windows (left and right of main door)
    drawGlassWindow(-5.20f, 1.10f, 4.02f, 2.20f, 1.60f, false);
    drawGlassWindow( 2.40f, 1.10f, 4.02f, 2.20f, 1.60f, false);

    // Ground floor side windows
    drawGlassWindow(-6.02f, 1.10f, -2.60f, 2.00f, 1.60f, true);
    drawGlassWindow(-6.02f, 1.10f,  0.60f, 2.00f, 1.60f, true);

    // Ground Floor Furniture
    drawFloorFurniture(0, gFloorY + 0.15f, gCeilY);


    // ---------------------------------------------------------------------
    // UPPER FLOORS: 1ST, 2ND, 3RD, 4TH FLOORS
    // ARCHITECTURAL SEPARATION:
    // LEFT SIDE: LARGE GLASS WINDOW (JANALA)
    // RIGHT SIDE: DEDICATED BALCONY WITH DOOR (BELKONI)
    // ---------------------------------------------------------------------
    for (int floor = 1; floor <= 4; floor++)
    {
        GLfloat baseY = 0.40f + (GLfloat)floor * 3.00f;
        GLfloat floorY = baseY + 0.15f;
        GLfloat ceilY  = baseY + 2.85f;

        // Floor division slab / Cornice band (white architectural trim)
        drawSolidBox(-6.25f, baseY - 0.15f, -4.25f, 12.50f, 0.30f, 8.50f, 0.96f, 0.96f, 0.98f);

        // Floor tile surface & ceiling slab
        drawSolidBox(-6.0f, baseY, -4.0f, 12.0f, 0.15f, 8.0f, 0.88f, 0.86f, 0.82f);
        drawSolidBox(-6.0f, ceilY, -4.0f, 12.0f, 0.15f, 8.0f, 0.92f, 0.92f, 0.92f);

        // Back wall
        drawSolidBox(-6.0f, baseY, -4.0f, 12.0f, 2.85f, 0.20f, 0.92f, 0.90f, 0.85f);
        // Left side wall
        drawSolidBox(-6.0f, baseY, -4.0f, 0.20f, 2.85f, 8.0f, 0.92f, 0.90f, 0.85f);
        // Right side wall
        drawSolidBox(5.80f, baseY, -4.0f, 0.20f, 2.85f, 8.0f, 0.92f, 0.90f, 0.85f);

        // Front wall segments:
        // Left side solid wall (Window area): X = -6.0 to 1.0
        drawSolidBox(-6.0f, baseY, 3.80f, 7.00f, 2.85f, 0.20f, 0.92f, 0.90f, 0.85f);
        // Right side wall (supporting balcony doorway): X = 1.0 to 2.0 and 4.0 to 5.8
        drawSolidBox(1.00f, baseY, 3.80f, 1.00f, 2.85f, 0.20f, 0.92f, 0.90f, 0.85f);
        drawSolidBox(4.00f, baseY, 3.80f, 1.80f, 2.85f, 0.20f, 0.92f, 0.90f, 0.85f);
        drawSolidBox(2.00f, baseY + 2.45f, 3.80f, 2.00f, 0.40f, 0.20f, 0.92f, 0.90f, 0.85f); // lintel above balcony door

        // -----------------------------------------------------------------
        // 1. LEFT SIDE: LARGE GLASS WINDOW (NO BALCONY ON THIS SIDE)
        // -----------------------------------------------------------------
        drawGlassWindow(-4.80f, baseY + 0.65f, 4.02f, 2.80f, 1.70f, false);

        // -----------------------------------------------------------------
        // 2. RIGHT SIDE: BALCONY WITH DOOR (NO WINDOW IN BALCONY)
        // -----------------------------------------------------------------
        drawBalcony(baseY);

        // Side glass windows on left wall
        drawGlassWindow(-6.02f, baseY + 0.70f, -2.60f, 2.00f, 1.60f, true);
        drawGlassWindow(-6.02f, baseY + 0.70f,  0.60f, 2.00f, 1.60f, true);

        // Draw Furniture for this floor
        drawFloorFurniture(floor, floorY, ceilY);
    }

    // ---------------------------------------------------------------------
    // CORNER PILLARS / COLUMNS
    // ---------------------------------------------------------------------
    drawSolidBox(-6.15f, 0.40f,  3.90f, 0.40f, 15.00f, 0.30f, 0.82f, 0.80f, 0.78f);
    drawSolidBox( 5.75f, 0.40f,  3.90f, 0.40f, 15.00f, 0.30f, 0.82f, 0.80f, 0.78f);
    drawSolidBox(-6.15f, 0.40f, -4.20f, 0.40f, 15.00f, 0.30f, 0.82f, 0.80f, 0.78f);
    drawSolidBox( 5.75f, 0.40f, -4.20f, 0.40f, 15.00f, 0.30f, 0.82f, 0.80f, 0.78f);

    // ---------------------------------------------------------------------
    // MULTI-LEVEL STAIRCASE SYSTEM (GROUND -> 1ST -> 2ND -> 3RD -> 4TH -> ROOF)
    // ---------------------------------------------------------------------
    drawStaircaseSystem();

    // ---------------------------------------------------------------------
    // ROOFTOP (SLAB, PARAPET WALL, MUMTY STAIR-ROOM, WATER RESERVOIR)
    // ---------------------------------------------------------------------
    drawRooftop();
}


/* =========================================================================
   OPENGL DISPLAY CALLBACK FUNCTION
   ========================================================================= */

void display(void)
{
    // Clear color buffer and depth buffer (Lab-01 p4 & p5)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Viewport mapping (Lab-01 p5)
    glViewport(0, 0, windowWidth, windowHeight);

    // Projection Matrix setup with dynamic FOV zoom (Lab-01 p5 & Lab-02 p3)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (GLdouble)windowWidth / (GLdouble)windowHeight, 1.0, 500.0);

    // Modelview Matrix setup (Lab-01 p5 & Lab-02 p3)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Update target point based on current eye position and look angles
    updateCameraLook();

    // Camera viewing coordinate system with Eye, LookAt & Head-Up vector (Lab-02 p3)
    gluLookAt(eyeX, eyeY, eyeZ,
              lookX, lookY, lookZ,
              0.0, 1.0, 0.0);

    // Draw the entire 3D building model with furnished rooms
    drawScene();

    // Ensure all commands are executed and swap double buffers (Lab-01 p5 & p6)
    glFlush();
    glutSwapBuffers();
}


/* =========================================================================
   TERMINAL USER GUIDE / INSTRUCTION DISPLAY
   ========================================================================= */

void printInstructions()
{
    printf("\n");
    printf("======================================================================\n");
    printf(" 4-FLOOR 3D BUILDING DESIGN (ARCHITECTURAL FACADE & INTERIOR)         \n");
    printf("======================================================================\n");
    printf("  KEYBOARD CONTROLS (TERMINAL GUIDE):                                 \n");
    printf("----------------------------------------------------------------------\n");
    printf("  [a / A]      : Move / Strafe LEFT     (Bame jabe)                   \n");
    printf("  [d / D]      : Move / Strafe RIGHT    (Dane jabe)                   \n");
    printf("  [w / W]      : Walk TOP / Forward     (Ghore dhukte samne jabe)     \n");
    printf("  [s / S]      : Walk BOTTOM / Backward (Pichone jabe)                \n");
    printf("  [LEFT Arrow] : Look LEFT (Ek jaigai theke BAME takabe)              \n");
    printf("  [RIGHT Arrow]: Look RIGHT (Ek jaigai theke DANE takabe)             \n");
    printf("  [UP Arrow]   : Look UP (Ek jaigai theke UPORE takabe)               \n");
    printf("  [DOWN Arrow] : Look DOWN (Ek jaigai theke NICHE takabe)             \n");
    printf("  [r / R]      : Rise UP Eye Level      (Upore uthe onno floor dekha) \n");
    printf("  [f / F]      : Fall DOWN Eye Level    (Niche nambe)                 \n");
    printf("  [+ / =]      : ZOOM IN                (Lens optical zoom in)        \n");
    printf("  [- / _]      : ZOOM OUT               (Lens optical zoom out)       \n");
    printf("  [0]          : RESET VIEW             (Default viewpoint e ferot)   \n");
    printf("  [ESC]        : Exit Application                                     \n");
    printf("======================================================================\n");
    printf("Facade Design: Left Wing = Glass Window | Right Wing = Balcony & Door \n");
    printf("Room Furnishings: AC, Bed, Sofa, Table, Chairs, Fan, LED Lights, TV!  \n");
    printf("======================================================================\n\n");
}


/* =========================================================================
   KEYBOARD INPUT HANDLER: glutKeyboardFunc (LAB-01 PAGE 8 & 9)
   ========================================================================= */

void myKeyboardFunc(unsigned char key, int x, int y)
{
    switch (key)
    {
        // a = Move / Strafe LEFT relative to current look direction
        case 'a':
        case 'A':
            eyeX -= (GLfloat)(cos(yaw)) * moveSpeed;
            eyeZ -= (GLfloat)(sin(yaw)) * moveSpeed;
            updateCameraLook();
            printf("[KEY '%c'] Move LEFT (Strafe)  | Eye: (%5.1f, %5.1f, %5.1f)\n",
                   key, eyeX, eyeY, eyeZ);
            break;

        // d = Move / Strafe RIGHT relative to current look direction
        case 'd':
        case 'D':
            eyeX += (GLfloat)(cos(yaw)) * moveSpeed;
            eyeZ += (GLfloat)(sin(yaw)) * moveSpeed;
            updateCameraLook();
            printf("[KEY '%c'] Move RIGHT (Strafe) | Eye: (%5.1f, %5.1f, %5.1f)\n",
                   key, eyeX, eyeY, eyeZ);
            break;

        // w = Walk TOP / Forward into the rooms
        case 'w':
        case 'W':
            eyeX += (GLfloat)(sin(yaw)) * moveSpeed;
            eyeZ -= (GLfloat)(cos(yaw)) * moveSpeed;
            updateCameraLook();
            printf("[KEY '%c'] Walk TOP / Forward  | Eye: (%5.1f, %5.1f, %5.1f)\n",
                   key, eyeX, eyeY, eyeZ);
            break;

        // s = Walk BOTTOM / Backward
        case 's':
        case 'S':
            eyeX -= (GLfloat)(sin(yaw)) * moveSpeed;
            eyeZ += (GLfloat)(cos(yaw)) * moveSpeed;
            updateCameraLook();
            printf("[KEY '%c'] Walk BOTTOM / Back  | Eye: (%5.1f, %5.1f, %5.1f)\n",
                   key, eyeX, eyeY, eyeZ);
            break;

        // r = Rise UP eye level (elevate height to inspect upper floors)
        case 'r':
        case 'R':
            eyeY += moveSpeed;
            updateCameraLook();
            printf("[KEY '%c'] Rise UP Height      | Eye: (%5.1f, %5.1f, %5.1f)\n",
                   key, eyeX, eyeY, eyeZ);
            break;

        // f = Fall DOWN eye level (descend height to ground floor)
        case 'f':
        case 'F':
            if (eyeY > 0.8f) eyeY -= moveSpeed;
            updateCameraLook();
            printf("[KEY '%c'] Fall DOWN Height    | Eye: (%5.1f, %5.1f, %5.1f)\n",
                   key, eyeX, eyeY, eyeZ);
            break;

        // + = Optical Zoom IN
        case '+':
        case '=':
            if (fov > 12.0) fov -= 2.0;
            printf("[KEY '%c'] ZOOM IN (Optical)   | FOV: %4.1f deg\n", key, fov);
            break;

        // - = Optical Zoom OUT
        case '-':
        case '_':
            if (fov < 95.0) fov += 2.0;
            printf("[KEY '%c'] ZOOM OUT (Optical)  | FOV: %4.1f deg\n", key, fov);
            break;

        // 0 = Reset Viewpoint to default
        case '0':
            eyeX  = defaultEyeX;
            eyeY  = defaultEyeY;
            eyeZ  = defaultEyeZ;
            yaw   = defaultYaw;
            pitch = defaultPitch;
            fov   = defaultFov;
            updateCameraLook();
            printf("[KEY '0'] RESET VIEW TO DEFAULT | Eye: (%5.1f, %5.1f, %5.1f), Yaw: 0 deg, Pitch: 0 deg\n",
                   eyeX, eyeY, eyeZ);
            break;

        case 27: // Escape key (Lab-01 p8)
            printf("[ESC] Exiting program. Goodbye!\n");
            exit(0);
            break;
    }

    // Refresh the display window (Lab-01 p9)
    glutPostRedisplay();
}


/* =========================================================================
   SPECIAL KEYBOARD INPUT HANDLER: glutSpecialFunc (FOR ARROW KEYS)
   EK JAIGAI THEKE LEFT, RIGHT, UP, DOWN ER DIKE TAKATE PARBO!
   ========================================================================= */

void mySpecialFunc(int key, int x, int y)
{
    switch (key)
    {
        // Left Arrow: Stand in place & look LEFT (turn head left)
        case GLUT_KEY_LEFT:
            yaw -= angleSpeed;
            updateCameraLook();
            printf("[LEFT ARROW]  Look LEFT (Ek jaigai theke)  | Yaw: %5.1f deg | Eye: (%5.1f, %5.1f, %5.1f)\n",
                   (yaw * 180.0f / 3.14159f), eyeX, eyeY, eyeZ);
            break;

        // Right Arrow: Stand in place & look RIGHT (turn head right)
        case GLUT_KEY_RIGHT:
            yaw += angleSpeed;
            updateCameraLook();
            printf("[RIGHT ARROW] Look RIGHT (Ek jaigai theke) | Yaw: %5.1f deg | Eye: (%5.1f, %5.1f, %5.1f)\n",
                   (yaw * 180.0f / 3.14159f), eyeX, eyeY, eyeZ);
            break;

        // Up Arrow: Stand in place & look UP (tilt head up towards roof/ceiling/fan)
        case GLUT_KEY_UP:
            if (pitch < 1.48f) pitch += angleSpeed;
            updateCameraLook();
            printf("[UP ARROW]    Look UP (Ek jaigai theke)    | Pitch: %5.1f deg | Eye: (%5.1f, %5.1f, %5.1f)\n",
                   (pitch * 180.0f / 3.14159f), eyeX, eyeY, eyeZ);
            break;

        // Down Arrow: Stand in place & look DOWN (tilt head down towards furniture/floor)
        case GLUT_KEY_DOWN:
            if (pitch > -1.48f) pitch -= angleSpeed;
            updateCameraLook();
            printf("[DOWN ARROW]  Look DOWN (Ek jaigai theke)  | Pitch: %5.1f deg | Eye: (%5.1f, %5.1f, %5.1f)\n",
                   (pitch * 180.0f / 3.14159f), eyeX, eyeY, eyeZ);
            break;
    }

    // Refresh the display window (Lab-01 p9)
    glutPostRedisplay();
}


/* =========================================================================
   MAIN FUNCTION (FROM LAB-01 PAGE 6 & LAB-02)
   ========================================================================= */

int main(int argc, char **argv)
{
    // Initialize GLUT library with command line parameters (Lab-01 p6)
    glutInit(&argc, argv);

    // Display mode: Double buffering, RGB color, Depth testing (Lab-01 p6)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Initial window position on screen (Lab-01 p6 & p7)
    glutInitWindowPosition(100, 100);

    // Initial window dimensions (Lab-01 p6 & p7)
    glutInitWindowSize(windowWidth, windowHeight);

    // Create window with specified title (Lab-01 p6 & p7)
    glutCreateWindow("4-Floor 3D Building Design - KUET CSE");

    // Enable smooth shading (Lab-01 p6 & p7)
    glShadeModel(GL_SMOOTH);

    // Enable depth test for proper 3D rendering (Lab-01 p6 & p7)
    glEnable(GL_DEPTH_TEST);

    // Enable normal normalization (Lab-02 p3)
    glEnable(GL_NORMALIZE);

    // Register display callback function (Lab-01 p6 & p7)
    glutDisplayFunc(display);

    // Register keyboard callbacks for user controls (Lab-01 p8 & p9)
    glutKeyboardFunc(myKeyboardFunc);
    glutSpecialFunc(mySpecialFunc);

    // Initial calculation of line of sight
    updateCameraLook();

    // Print command guide to console terminal
    printInstructions();

    // Enter GLUT event processing loop (Lab-01 p6 & p7)
    glutMainLoop();

    return 0;
}
