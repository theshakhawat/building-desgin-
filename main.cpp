#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>

/* =========================================================================
   1. GLOBAL VARIABLES & 360-DEGREE CAMERA STATE
   ========================================================================= */

int windowWidth = 900;
int windowHeight = 650;

// Viewer eye position in 3D world
GLfloat eyeX = 0.0f;
GLfloat eyeY = 4.5f;
GLfloat eyeZ = 22.0f;

// 360-Degree Camera Angles (Yaw = Horizontal 360 rotation, Pitch = Vertical tilt)
GLfloat yaw = -90.0f;   // -90 degrees points directly towards -Z (front of the house)
GLfloat pitch = 0.0f;   // 0 degrees is level horizontal view

// Field of view for zoom (Lab-02 p3 gluPerspective)
GLdouble fovy = 45.0;

// Default values for resetting with '0'
const GLfloat defEyeX = 0.0f;
const GLfloat defEyeY = 4.5f;
const GLfloat defEyeZ = 22.0f;
const GLfloat defYaw = -90.0f;
const GLfloat defPitch = 0.0f;
const GLdouble defFovy = 45.0;

const GLfloat moveStep = 0.8f; // Movement step for WASD
const GLfloat rotStep = 4.0f;  // Rotation step for Arrow Keys (degrees per press)
const float PI = 3.14159265f;


/* =========================================================================
   2. GEOMETRY DATA & NORMAL CALCULATION (LAB-02 PAGE 1 & 2)
   ========================================================================= */

// 8 vertices for a 1x1x1 unit cube (Lab-02 p1)
static GLfloat v_cube[8][3] = {
    {0.0f, 0.0f, 0.0f}, // 0
    {1.0f, 0.0f, 0.0f}, // 1
    {1.0f, 1.0f, 0.0f}, // 2
    {0.0f, 1.0f, 0.0f}, // 3
    {0.0f, 0.0f, 1.0f}, // 4
    {1.0f, 0.0f, 1.0f}, // 5
    {1.0f, 1.0f, 1.0f}, // 6
    {0.0f, 1.0f, 1.0f}  // 7
};

// 6 quad faces with anti-clockwise vertices (Lab-02 p1)
static GLubyte cube_indices[6][4] = {
    {1, 0, 3, 2}, // Back face   (normal: -Z)
    {4, 5, 6, 7}, // Front face  (normal: +Z)
    {0, 1, 5, 4}, // Bottom face (normal: -Y)
    {7, 6, 2, 3}, // Top face    (normal: +Y)
    {0, 4, 7, 3}, // Left face   (normal: -X)
    {5, 1, 2, 6}  // Right face  (normal: +X)
};

// Normal vector calculation via cross product (Lab-02 p2)
static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1,
                        GLfloat x2, GLfloat y2, GLfloat z2,
                        GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux = x2 - x1, Uy = y2 - y1, Uz = z2 - z1;
    GLfloat Vx = x3 - x1, Vy = y3 - y1, Vz = z3 - z1;
    GLfloat Nx = Uy * Vz - Uz * Vy;
    GLfloat Ny = Uz * Vx - Ux * Vz;
    GLfloat Nz = Ux * Vy - Uy * Vx;
    glNormal3f(Nx, Ny, Nz);
}

// Draws a unit cube using GL_QUADS and calculated normals (Lab-02 p2)
void drawUnitCube()
{
    glBegin(GL_QUADS);
    for (int i = 0; i < 6; i++) {
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

// Primary building block: draws transformed colored box using unit cube
void drawBox(GLfloat x, GLfloat y, GLfloat z,
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
   3. PARKED CAR IN GARAGE (ALL MADE OF CUBES)
   ========================================================================= */

void drawParkedCar(GLfloat x, GLfloat y, GLfloat z)
{
    // 1. Car Lower Chassis / Body (Red Sporty Finish)
    drawBox(x, y + 0.25f, z, 2.2f, 0.45f, 4.2f, 0.85f, 0.15f, 0.15f);

    // 2. Front Hood & Rear Trunk Lower Trim
    drawBox(x + 0.1f, y + 0.70f, z + 0.2f, 2.0f, 0.05f, 1.2f, 0.75f, 0.12f, 0.12f); // Rear trunk
    drawBox(x + 0.1f, y + 0.70f, z + 2.8f, 2.0f, 0.05f, 1.2f, 0.75f, 0.12f, 0.12f); // Front hood

    // 3. Cabin / Roof (Darker Red / Frame)
    drawBox(x + 0.15f, y + 0.70f, z + 1.2f, 1.9f, 0.65f, 1.8f, 0.70f, 0.10f, 0.10f);

    // 4. Windshield Glass (Front, Rear, and Side Windows - Cyan Blue)
    drawBox(x + 0.2f, y + 0.72f, z + 2.95f, 1.8f, 0.58f, 0.08f, 0.40f, 0.75f, 0.95f); // Front Windshield
    drawBox(x + 0.2f, y + 0.72f, z + 1.17f, 1.8f, 0.58f, 0.08f, 0.40f, 0.75f, 0.95f); // Rear Windshield
    drawBox(x + 0.12f, y + 0.75f, z + 1.35f, 0.05f, 0.52f, 1.5f, 0.40f, 0.75f, 0.95f); // Left Windows
    drawBox(x + 2.03f, y + 0.75f, z + 1.35f, 0.05f, 0.52f, 1.5f, 0.40f, 0.75f, 0.95f); // Right Windows

    // 5. Headlights & Taillights
    drawBox(x + 0.25f, y + 0.40f, z + 4.22f, 0.45f, 0.20f, 0.05f, 1.0f, 0.95f, 0.3f);  // Front Left Light
    drawBox(x + 1.50f, y + 0.40f, z + 4.22f, 0.45f, 0.20f, 0.05f, 1.0f, 0.95f, 0.3f);  // Front Right Light
    drawBox(x + 0.25f, y + 0.40f, z - 0.05f, 0.45f, 0.18f, 0.05f, 0.95f, 0.1f, 0.1f);  // Rear Left Light
    drawBox(x + 1.50f, y + 0.40f, z - 0.05f, 0.45f, 0.18f, 0.05f, 0.95f, 0.1f, 0.1f);  // Rear Right Light

    // 6. Front & Rear Bumpers (Metallic Gray)
    drawBox(x - 0.05f, y + 0.18f, z + 4.18f, 2.3f, 0.18f, 0.12f, 0.3f, 0.3f, 0.32f);
    drawBox(x - 0.05f, y + 0.18f, z - 0.10f, 2.3f, 0.18f, 0.12f, 0.3f, 0.3f, 0.32f);

    // 7. 4 Wheels (Black Tires with Silver Rims)
    // Front-Left Wheel
    drawBox(x - 0.12f, y, z + 3.0f, 0.22f, 0.45f, 0.55f, 0.12f, 0.12f, 0.12f);
    drawBox(x - 0.14f, y + 0.1f, z + 3.1f, 0.05f, 0.25f, 0.35f, 0.8f, 0.8f, 0.85f);
    // Front-Right Wheel
    drawBox(x + 2.10f, y, z + 3.0f, 0.22f, 0.45f, 0.55f, 0.12f, 0.12f, 0.12f);
    drawBox(x + 2.29f, y + 0.1f, z + 3.1f, 0.05f, 0.25f, 0.35f, 0.8f, 0.8f, 0.85f);
    // Rear-Left Wheel
    drawBox(x - 0.12f, y, z + 0.6f, 0.22f, 0.45f, 0.55f, 0.12f, 0.12f, 0.12f);
    drawBox(x - 0.14f, y + 0.1f, z + 0.7f, 0.05f, 0.25f, 0.35f, 0.8f, 0.8f, 0.85f);
    // Rear-Right Wheel
    drawBox(x + 2.10f, y, z + 0.6f, 0.22f, 0.45f, 0.55f, 0.12f, 0.12f, 0.12f);
    drawBox(x + 2.29f, y + 0.1f, z + 0.7f, 0.05f, 0.25f, 0.35f, 0.8f, 0.8f, 0.85f);
}


/* =========================================================================
   4. INTERIOR FURNITURE & AMENITIES (ALL MADE OF CUBES)
   ========================================================================= */

// Bed with wooden frame, mattress, blanket, and pillows
void drawBed(GLfloat x, GLfloat y, GLfloat z)
{
    drawBox(x, y, z, 2.6f, 0.35f, 3.2f, 0.45f, 0.22f, 0.12f);          // Base wooden frame
    drawBox(x + 0.1f, y + 0.35f, z + 0.1f, 2.4f, 0.25f, 3.0f, 0.95f, 0.95f, 0.95f); // Mattress
    drawBox(x + 0.1f, y + 0.38f, z + 0.1f, 2.4f, 0.25f, 2.1f, 0.82f, 0.22f, 0.25f); // Red Blanket
    drawBox(x, y, z + 3.05f, 2.6f, 1.1f, 0.15f, 0.38f, 0.18f, 0.08f);          // Headboard
    drawBox(x + 0.25f, y + 0.62f, z + 2.4f, 0.85f, 0.12f, 0.5f, 1.0f, 1.0f, 1.0f);  // Pillow 1
    drawBox(x + 1.50f, y + 0.62f, z + 2.4f, 0.85f, 0.12f, 0.5f, 1.0f, 1.0f, 1.0f);  // Pillow 2
}

// Living Room Sofa Set
void drawSofa(GLfloat x, GLfloat y, GLfloat z)
{
    drawBox(x, y, z, 2.8f, 0.40f, 1.2f, 0.22f, 0.45f, 0.75f);            // Seat
    drawBox(x, y + 0.40f, z, 2.8f, 0.70f, 0.30f, 0.18f, 0.38f, 0.65f);    // Backrest
    drawBox(x - 0.22f, y, z, 0.22f, 0.60f, 1.2f, 0.18f, 0.38f, 0.65f);    // Left Arm
    drawBox(x + 2.80f, y, z, 0.22f, 0.60f, 1.2f, 0.18f, 0.38f, 0.65f);    // Right Arm
    drawBox(x + 0.3f, y + 0.42f, z + 0.3f, 0.6f, 0.35f, 0.15f, 0.95f, 0.75f, 0.2f); // Cushion 1
    drawBox(x + 1.9f, y + 0.42f, z + 0.3f, 0.6f, 0.35f, 0.15f, 0.95f, 0.75f, 0.2f); // Cushion 2
}

// Table and Chair Set
void drawTableChair(GLfloat x, GLfloat y, GLfloat z)
{
    // Tabletop & 4 wooden legs
    drawBox(x, y + 0.75f, z, 1.8f, 0.08f, 1.2f, 0.48f, 0.28f, 0.14f);
    drawBox(x + 0.1f, y, z + 0.1f, 0.08f, 0.75f, 0.08f, 0.32f, 0.16f, 0.08f);
    drawBox(x + 1.6f, y, z + 0.1f, 0.08f, 0.75f, 0.08f, 0.32f, 0.16f, 0.08f);
    drawBox(x + 0.1f, y, z + 1.0f, 0.08f, 0.75f, 0.08f, 0.32f, 0.16f, 0.08f);
    drawBox(x + 1.6f, y, z + 1.0f, 0.08f, 0.75f, 0.08f, 0.32f, 0.16f, 0.08f);

    // Chair
    drawBox(x + 0.6f, y + 0.42f, z + 1.35f, 0.55f, 0.06f, 0.45f, 0.32f, 0.16f, 0.08f); // Seat
    drawBox(x + 0.6f, y + 0.48f, z + 1.75f, 0.55f, 0.48f, 0.06f, 0.32f, 0.16f, 0.08f); // Back
    drawBox(x + 0.6f, y, z + 1.35f, 0.06f, 0.42f, 0.06f, 0.22f, 0.11f, 0.05f);         // Leg 1
    drawBox(x + 1.1f, y, z + 1.35f, 0.06f, 0.42f, 0.06f, 0.22f, 0.11f, 0.05f);         // Leg 2
}

// Split Air Conditioner (AC)
void drawAC(GLfloat x, GLfloat y, GLfloat z)
{
    drawBox(x, y, z, 1.5f, 0.42f, 0.28f, 0.95f, 0.95f, 0.97f);          // White body
    drawBox(x + 0.1f, y + 0.04f, z + 0.26f, 1.3f, 0.05f, 0.03f, 0.3f, 0.3f, 0.35f); // Vent flap
    drawBox(x + 1.15f, y + 0.20f, z + 0.27f, 0.16f, 0.07f, 0.02f, 0.2f, 0.85f, 0.2f); // LED display
}

// Ceiling Fan
void drawFan(GLfloat x, GLfloat y, GLfloat z)
{
    drawBox(x - 0.02f, y - 0.35f, z - 0.02f, 0.04f, 0.35f, 0.04f, 0.2f, 0.2f, 0.2f); // Rod
    drawBox(x - 0.20f, y - 0.45f, z - 0.20f, 0.40f, 0.10f, 0.40f, 0.85f, 0.75f, 0.2f); // Motor dome
    // 4 Blades
    drawBox(x + 0.20f, y - 0.43f, z - 0.08f, 0.90f, 0.02f, 0.16f, 0.42f, 0.22f, 0.12f);
    drawBox(x - 1.10f, y - 0.43f, z - 0.08f, 0.90f, 0.02f, 0.16f, 0.42f, 0.22f, 0.12f);
    drawBox(x - 0.08f, y - 0.43f, z + 0.20f, 0.16f, 0.02f, 0.90f, 0.42f, 0.22f, 0.12f);
    drawBox(x - 0.08f, y - 0.43f, z - 1.10f, 0.16f, 0.02f, 0.90f, 0.42f, 0.22f, 0.12f);
}

// Ceiling Light Panel
void drawLight(GLfloat x, GLfloat y, GLfloat z)
{
    drawBox(x - 0.25f, y - 0.04f, z - 0.25f, 0.50f, 0.04f, 0.50f, 1.0f, 0.96f, 0.65f);
}

// Glass Window with Frame and Sunshade
void drawWindow(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h)
{
    drawBox(x, y, z, w, h, 0.10f, 0.20f, 0.20f, 0.22f);                     // Dark frame
    drawBox(x + 0.08f, y + 0.08f, z + 0.02f, w - 0.16f, h - 0.16f, 0.06f, 0.40f, 0.75f, 0.95f); // Sky-blue Glass
    drawBox(x - 0.08f, y + h, z - 0.05f, w + 0.16f, 0.10f, 0.35f, 0.85f, 0.85f, 0.88f);        // Sunshade
}

// Wooden Door with Brass Handle
void drawDoor(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h)
{
    drawBox(x, y, z, w, h, 0.12f, 0.28f, 0.14f, 0.06f);                    // Frame
    drawBox(x + 0.06f, y, z + 0.02f, w - 0.12f, h - 0.04f, 0.08f, 0.52f, 0.26f, 0.12f); // Wooden panel
    drawBox(x + w * 0.75f, y + h * 0.45f, z + 0.10f, 0.05f, 0.22f, 0.05f, 0.95f, 0.80f, 0.20f); // Handle
}


/* =========================================================================
   5. DUPLEX HOUSE SCENE ASSEMBLY (2 FLOORS + GARAGE + PARKED CAR)
   ========================================================================= */

void drawDuplexHouse()
{
    // ---------------------------------------------------------------------
    // A. GROUND & PLINTH FOUNDATION
    // ---------------------------------------------------------------------
    // Green Garden Lawn
    drawBox(-16.0f, -0.2f, -12.0f, 32.0f, 0.2f, 26.0f, 0.28f, 0.65f, 0.28f);
    // Garage Driveway / Pavement
    drawBox(1.5f, -0.05f, 4.0f, 5.5f, 0.08f, 8.0f, 0.55f, 0.55f, 0.58f);
    // Front Walkway to Main Door
    drawBox(-3.5f, -0.05f, 4.0f, 3.0f, 0.08f, 8.0f, 0.70f, 0.70f, 0.72f);
    // House Foundation Plinth
    drawBox(-7.2f, 0.0f, -5.2f, 14.4f, 0.4f, 10.4f, 0.45f, 0.45f, 0.48f);

    // ---------------------------------------------------------------------
    // B. GROUND FLOOR (Floor 1: Y = 0.4 to 3.6)
    // Left Wing: Living Room | Right Wing: Garage with Parked Car
    // ---------------------------------------------------------------------
    GLfloat gY = 0.4f;

    // Floor Base Slab
    drawBox(-7.0f, gY, -5.0f, 14.0f, 0.15f, 10.0f, 0.88f, 0.86f, 0.82f);

    // Outer Walls (Back, Left, Right)
    drawBox(-7.0f, gY, -5.0f, 14.0f, 3.0f, 0.20f, 0.92f, 0.90f, 0.85f); // Back Wall
    drawBox(-7.0f, gY, -5.0f, 0.20f, 3.0f, 10.0f, 0.92f, 0.90f, 0.85f); // Left Wall
    drawBox( 6.8f, gY, -5.0f, 0.20f, 3.0f, 10.0f, 0.92f, 0.90f, 0.85f); // Right Wall

    // Partition Wall between Living Room & Garage
    drawBox(0.8f, gY, -5.0f, 0.20f, 3.0f, 10.0f, 0.90f, 0.88f, 0.84f);

    // 1. LEFT WING: LIVING ROOM (Front wall, entrance door, windows, sofa, TV/table)
    drawBox(-7.0f, gY, 4.8f, 4.2f, 3.0f, 0.20f, 0.92f, 0.90f, 0.85f); // Left front wall
    drawBox(-0.8f, gY, 4.8f, 1.8f, 3.0f, 0.20f, 0.92f, 0.90f, 0.85f); // Mid front wall
    drawDoor(-2.8f, gY, 4.88f, 2.0f, 2.4f);                            // Main Entrance Door
    drawWindow(-6.2f, gY + 0.8f, 4.95f, 2.2f, 1.5f);                   // Front Window

    // Living Room Furniture
    drawSofa(-5.8f, gY + 0.15f, 0.0f);                                  // Blue Sofa Set
    drawTableChair(-5.5f, gY + 0.15f, -3.8f);                           // Table & Chair
    drawAC(-6.8f, gY + 2.2f, -1.0f);                                    // Split AC on left wall
    drawFan(-3.0f, gY + 3.0f, 0.0f);                                    // Ceiling Fan
    drawLight(-3.0f, gY + 3.0f, 0.0f);                                  // Ceiling Light

    // 2. RIGHT WING: GARAGE (Open front with pillars & Parked Car)
    drawBox(1.0f, gY, 4.8f, 0.5f, 3.0f, 0.20f, 0.85f, 0.85f, 0.88f);  // Garage Left Pillar
    drawBox(6.5f, gY, 4.8f, 0.5f, 3.0f, 0.20f, 0.85f, 0.85f, 0.88f);  // Garage Right Pillar
    drawBox(1.0f, gY + 2.6f, 4.8f, 6.0f, 0.4f, 0.20f, 0.85f, 0.85f, 0.88f); // Garage Header Beam

    // Parked Car inside Garage!
    drawParkedCar(2.4f, gY + 0.15f, -1.5f);

    // Duplex Internal Staircase climbing to 1st Floor
    for (int s = 0; s < 6; s++) {
        drawBox(-1.0f, gY + (GLfloat)s * 0.50f, -4.5f + (GLfloat)s * 0.65f, 1.6f, 0.50f, 0.65f, 0.78f, 0.78f, 0.82f);
    }

    // ---------------------------------------------------------------------
    // C. 1ST FLOOR / UPPER DUPLEX FLOOR (Floor 2: Y = 3.4 to 6.6)
    // Left Wing: Master Bedroom | Right Wing: Open Balcony / Terrace
    // ---------------------------------------------------------------------
    GLfloat f2Y = 3.4f;

    // Floor Division Slab (Trim)
    drawBox(-7.2f, f2Y, -5.2f, 14.4f, 0.30f, 10.4f, 0.95f, 0.95f, 0.98f);

    // 1. MASTER BEDROOM (Left Wing: X = -7.0 to 1.0)
    drawBox(-7.0f, f2Y + 0.3f, -5.0f, 8.0f, 2.85f, 0.20f, 0.92f, 0.90f, 0.85f); // Back Wall
    drawBox(-7.0f, f2Y + 0.3f, -5.0f, 0.20f, 2.85f, 10.0f, 0.92f, 0.90f, 0.85f); // Left Wall
    drawBox( 0.8f, f2Y + 0.3f, -5.0f, 0.20f, 2.85f, 10.0f, 0.92f, 0.90f, 0.85f); // Right Wall
    drawBox(-7.0f, f2Y + 0.3f,  4.8f, 8.0f, 2.85f, 0.20f, 0.92f, 0.90f, 0.85f); // Front Wall

    // Bedroom Front Window
    drawWindow(-5.5f, f2Y + 1.0f, 4.95f, 3.0f, 1.6f);

    // Bedroom Furniture
    drawBed(-5.8f, f2Y + 0.3f, -4.2f);                                  // King Bed
    drawTableChair(-2.0f, f2Y + 0.3f, -2.0f);                           // Study Table & Chair
    drawAC(-6.8f, f2Y + 2.2f, -1.0f);                                   // Bedroom AC
    drawFan(-3.0f, f2Y + 3.1f, 0.0f);                                   // Ceiling Fan
    drawLight(-3.0f, f2Y + 3.1f, 0.0f);                                 // Ceiling Light

    // 2. RIGHT WING: UPPER BALCONY / ROOF TERRACE OVER GARAGE
    // Glass railing and handrails around garage terrace
    drawBox(1.0f, f2Y + 0.3f, 4.9f, 6.0f, 0.80f, 0.06f, 0.45f, 0.75f, 0.95f);  // Front Glass Railing
    drawBox(6.9f, f2Y + 0.3f, -5.0f, 0.06f, 0.80f, 10.0f, 0.45f, 0.75f, 0.95f); // Right Glass Railing
    drawBox(1.0f, f2Y + 1.1f, 4.9f, 6.0f, 0.06f, 0.06f, 0.30f, 0.30f, 0.35f);  // Handrail Front
    drawBox(6.9f, f2Y + 1.1f, -5.0f, 0.06f, 0.06f, 10.0f, 0.30f, 0.30f, 0.35f); // Handrail Right

    // Door leading from Master Bedroom out to Balcony Terrace
    drawDoor(0.9f, f2Y + 0.3f, 1.5f, 0.12f, 2.2f);

    // ---------------------------------------------------------------------
    // D. ROOFTOP (Y = 6.55 to 8.5)
    // ---------------------------------------------------------------------
    GLfloat rY = 6.55f;

    // Roof Slab
    drawBox(-7.2f, rY, -5.2f, 8.4f, 0.25f, 10.4f, 0.88f, 0.88f, 0.90f);

    // Parapet Boundary Safety Wall
    drawBox(-7.2f, rY + 0.25f,  5.0f, 8.4f, 0.70f, 0.20f, 0.90f, 0.88f, 0.84f);
    drawBox(-7.2f, rY + 0.25f, -5.2f, 8.4f, 0.70f, 0.20f, 0.90f, 0.88f, 0.84f);
    drawBox(-7.2f, rY + 0.25f, -5.2f, 0.20f, 0.70f, 10.4f, 0.90f, 0.88f, 0.84f);
    drawBox( 1.0f, rY + 0.25f, -5.2f, 0.20f, 0.70f, 10.4f, 0.90f, 0.88f, 0.84f);

    // Blue Overhead Water Tank
    drawBox(-5.0f, rY + 0.25f, -3.5f, 2.2f, 1.4f, 2.2f, 0.18f, 0.52f, 0.82f);
}


/* =========================================================================
   6. OPENGL DISPLAY CALLBACK FUNCTION (LAB-01 P5 & LAB-02 P3)
   ========================================================================= */

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, windowWidth, windowHeight);

    // Projection setup with zoom (Lab-01 p5 & Lab-02 p3)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (GLdouble)windowWidth / (GLdouble)windowHeight, 1.0, 500.0);

    // 360-degree look direction vector calculation
    float radYaw = yaw * PI / 180.0f;
    float radPitch = pitch * PI / 180.0f;

    float dirX = cosf(radPitch) * cosf(radYaw);
    float dirY = sinf(radPitch);
    float dirZ = cosf(radPitch) * sinf(radYaw);

    // Modelview camera setup (Lab-01 p5 & Lab-02 p3)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ,
              eyeX + dirX, eyeY + dirY, eyeZ + dirZ,
              0.0, 1.0, 0.0);

    // Draw the 2-story Duplex House with Garage & Parked Car
    drawDuplexHouse();

    glFlush();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = (h > 0) ? h : 1;
    glViewport(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight);
}


/* =========================================================================
   7. TERMINAL USER GUIDE DISPLAY
   ========================================================================= */

void printInstructions()
{
    printf("\n");
    printf("======================================================================\n");
    printf("   2-STORY DUPLEX HOUSE WITH GARAGE & PARKED CAR (360 VIEW)           \n");
    printf("   KUET CSE COMPUTER GRAPHICS (LAB-01 & LAB-02)                       \n");
    printf("======================================================================\n");
    printf("  KEYBOARD CONTROLS:                                                  \n");
    printf("----------------------------------------------------------------------\n");
    printf("  [a / A]      : Move LEFT            (Bame jabe)                     \n");
    printf("  [d / D]      : Move RIGHT           (Dane jabe)                     \n");
    printf("  [w / W]      : Move FORWARD         (Samne jabe)                    \n");
    printf("  [s / S]      : Move BACKWARD        (Pichone jabe)                  \n");
    printf("  [LEFT Arrow] : 360 ROTATE LEFT      (360 degree Bame ghurbe)        \n");
    printf("  [RIGHT Arrow]: 360 ROTATE RIGHT     (360 degree Dane ghurbe)        \n");
    printf("  [UP Arrow]   : Look UP              (Uporer dike takabe)            \n");
    printf("  [DOWN Arrow] : Look DOWN            (Nicher dike takabe)            \n");
    printf("  [+ / =]      : ZOOM IN              (Zoom in hobe)                  \n");
    printf("  [- / _]      : ZOOM OUT             (Zoom out hobe)                 \n");
    printf("  [0]          : RESET Viewpoint      (Ager moto reset hobe)          \n");
    printf("  [ESC]        : Exit Application                                     \n");
    printf("======================================================================\n\n");
}


/* =========================================================================
   8. KEYBOARD INPUT CALLBACKS (LAB-01 PAGE 8 & 9)
   ========================================================================= */

void myKeyboardFunc(unsigned char key, int x, int y)
{
    float radYaw = yaw * PI / 180.0f;
    float forwardX = cosf(radYaw);
    float forwardZ = sinf(radYaw);
    float rightX = -sinf(radYaw);
    float rightZ = cosf(radYaw);

    switch (key)
    {
        case 'w': case 'W': // Move Forward in current look direction
            eyeX += forwardX * moveStep;
            eyeZ += forwardZ * moveStep;
            printf("[KEY '%c'] Move FORWARD  | Eye: (%.1f, %.1f, %.1f)\n", key, eyeX, eyeY, eyeZ);
            break;

        case 's': case 'S': // Move Backward
            eyeX -= forwardX * moveStep;
            eyeZ -= forwardZ * moveStep;
            printf("[KEY '%c'] Move BACKWARD | Eye: (%.1f, %.1f, %.1f)\n", key, eyeX, eyeY, eyeZ);
            break;

        case 'a': case 'A': // Strafe Left
            eyeX -= rightX * moveStep;
            eyeZ -= rightZ * moveStep;
            printf("[KEY '%c'] Move LEFT     | Eye: (%.1f, %.1f, %.1f)\n", key, eyeX, eyeY, eyeZ);
            break;

        case 'd': case 'D': // Strafe Right
            eyeX += rightX * moveStep;
            eyeZ += rightZ * moveStep;
            printf("[KEY '%c'] Move RIGHT    | Eye: (%.1f, %.1f, %.1f)\n", key, eyeX, eyeY, eyeZ);
            break;

        case '+': case '=': // Zoom In
            if (fovy > 10.0) fovy -= 2.0;
            printf("[KEY '%c'] Zoom IN       | FOV: %.1f deg\n", key, fovy);
            break;

        case '-': case '_': // Zoom Out
            if (fovy < 90.0) fovy += 2.0;
            printf("[KEY '%c'] Zoom OUT      | FOV: %.1f deg\n", key, fovy);
            break;

        case '0': // Reset to default
            eyeX = defEyeX; eyeY = defEyeY; eyeZ = defEyeZ;
            yaw = defYaw; pitch = defPitch;
            fovy = defFovy;
            printf("[KEY '0'] RESET Viewpoint to Default!\n");
            break;

        case 27: // Escape
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void mySpecialFunc(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:  // Continuous 360 Rotate Left
            yaw -= rotStep;
            if (yaw < -360.0f) yaw += 360.0f;
            printf("[LEFT ARROW]  360 Rotate LEFT  | Yaw: %.1f deg\n", yaw);
            break;

        case GLUT_KEY_RIGHT: // Continuous 360 Rotate Right
            yaw += rotStep;
            if (yaw > 360.0f) yaw -= 360.0f;
            printf("[RIGHT ARROW] 360 Rotate RIGHT | Yaw: %.1f deg\n", yaw);
            break;

        case GLUT_KEY_UP:    // Look Up (Pitch Up)
            if (pitch < 85.0f) pitch += rotStep;
            printf("[UP ARROW]    Look UP          | Pitch: %.1f deg\n", pitch);
            break;

        case GLUT_KEY_DOWN:  // Look Down (Pitch Down)
            if (pitch > -85.0f) pitch -= rotStep;
            printf("[DOWN ARROW]  Look DOWN        | Pitch: %.1f deg\n", pitch);
            break;
    }
    glutPostRedisplay();
}


/* =========================================================================
   9. MAIN FUNCTION (LAB-01 PAGE 6 & LAB-02)
   ========================================================================= */

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("2-Story Duplex House with Garage & Parked Car - KUET CSE");

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(myKeyboardFunc);
    glutSpecialFunc(mySpecialFunc);

    printInstructions();

    glutMainLoop();
    return 0;
}
