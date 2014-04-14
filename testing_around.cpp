// Authors:
// James Walker   jwwalker at mtu dot edu
// Scott A. Kuhl  kuhl at mtu dot edu

#include <GLUT/glut.h>
#include <math.h>
#include "DGR_framework.h"


#ifdef DGR_MASTER
#else // if SLAVE:


// command-line parameters
double frustum_left,frustum_right,frustum_bottom,frustum_top;
int screen_width,screen_height;
#endif

DGR_framework * myDGR;

void exitCallback(void){
    delete myDGR;
}

float data1 = 0.0f;
float data2 = 0.0f;
float data3 = 0.0f;
float data4 = 0.0f;
float data5 = 0.0f;

void init(void) 
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);
}

// OpenGL display function.
// Your graphics rendering code will go here.
// If you do any state updating in your display function, be sure to do it for the MASTER ONLY.
void display(void) 
{

#ifdef DGR_MASTER   // All code that updates state variables should be exclusive to the MASTER.
                    // Forbidding the SLAVES from updating state variables and only getting them
                    // from the MASTER is what guarantees that the processes all stay synchronized.
   

#else  // The slave automatically shuts itself off if it hasn't received
       // any packets within a few seconds (it gives itself longer if it
       // hasn't received any packets at all yet)
       // Assumes a 60fps framerate
    // framesPassed++;
    // if (myDGR->recvPack[0]) 
    // {
       
    //     if (framesPassed > 180) {
    //         //printf("DGR has revieved a packet and is timing out\n");
    //         exit(EXIT_SUCCESS);
    //     }
    // } 
    // else 
    // {
        
    //     if (framesPassed > 900){
    //     //printf("DGR has not revieved a packet and is timing out\n");
    //     exit(EXIT_SUCCESS); // If your program takes a very long time to initialize,
    //                                             // you can increase this value so the slaves don't prematurely
    //                                             // shut themselves off.
    //     }
    // }



      // Display code common to both the MASTER and SLAVE (except the frustum call)
      // This simple example just displays a wireframe cube and slowly rotates it.
    glClear (GL_COLOR_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);
    glLoadIdentity ();
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    float x = 0;
    float y = 0;
    float z = 1.5;

    glFrustum (frustum_left-x, frustum_right-x, frustum_bottom-z, frustum_top-z, 3.9-y, 5000); // edit the 0.1,5000 if you want to change the near/far clipping distance

    glMatrixMode (GL_MODELVIEW);
    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glTranslatef(0,0,-30);
    glScalef (8.0, 8.0, 8.0);
    glPushMatrix();
    glColor3ub(data2,data3,0);
    glRotatef(data1, data2, data3, data5);
    glutWireCube (1.0);
    glPopMatrix();
    glPushMatrix();
    glColor3ub(data2,data3,0);
    glRotatef(data1, 0.0f, 1.0f, 0.0f);
    glutWireCube(data4);
    glPopMatrix();

    glutSwapBuffers();
    glutPostRedisplay();
    #endif
}


// MAIN FUNCTION
int main(int argc, char** argv) 
{

#ifdef DGR_MASTER
    if (argc != 2) 
    {    
        printf("USAGE: %s relay-ip-address\n", argv[0]);
        return 1;
    }

    myDGR = new DGR_framework(argv[1]);

#else // if SLAVE:
    frustum_left = atof(argv[1]);
    frustum_right = atof(argv[2]);
    frustum_bottom = atof(argv[3]);
    frustum_top = atof(argv[4]);
    screen_width = atoi(argv[5]);
    screen_height = atoi(argv[6]);
    if(argc == 8){ myDGR = new DGR_framework( atoi(argv[7]) ); }
    else{ myDGR = new DGR_framework(); }
#endif
    //register variables
    myDGR->addNode<float>("data1",&data1);
    myDGR->addNode<float>("data2",&data2);
    myDGR->addNode<float>("data3",&data3);
    myDGR->addNode<float>("data4",&data4);



    

#ifdef DGR_MASTER
    while(true){
        data1 += 1.0f;
        data2 += 2.0f;
        data3 += 5.0f;
        data4 += 0.01f;
        data5 += 0.05f;
        usleep(32000);
    }
#else
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (screen_width, screen_height); 
    glutInitWindowPosition (0, 0);
    glutCreateWindow ("DGR Slave Node");


    init();
    glutDisplayFunc(display);
      //glutReshapeFunc(reshape);
      //glutIdleFunc(animate);

      // go


    glutMainLoop();

    exit(EXIT_SUCCESS);
    #endif
}
