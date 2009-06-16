
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#include "Observer.h"
using std::list;

using namespace NBMath;

// generated by octave
const float Observer::weights[NUM_AVAIL_PREVIEW_FRAMES] =
{
    28.7053f,
    45.1851f,
    47.2734f,
    44.5583f,
    40.542f,
    36.4072f,
    32.53f,
    29.009f,
    25.8492f,
    23.0268f,
    20.5101f,
    18.2677f,
    16.2701f,
    14.4909f,
    12.9062f,
    11.4948f,
    10.2377f,
    9.11811f,
    8.12095f,
    7.23284f,
    6.44186f,
    5.73738f,
    5.10994f,
    4.55112f,
    4.05341f,
    3.61013f,
    3.21532f,
    2.86369f,
    2.55052f,
    2.2716f,
    2.02317f,
    1.80192f,
    1.60486f,
    1.42935f,
    1.27304f,
    1.13382f,
    1.00983f,
    0.899391f,
    0.801034f,
    0.713433f,
    0.635412f,
    0.565923f,
    0.504034f,
    0.448913f,
    0.399819f,
    0.356095f,
    0.317153f,
    0.282469f,
    0.251578f,
    0.224065f,
    0.199562f,
    0.177738f,
    0.1583f,
    0.140988f,
    0.12557f,
    0.111838f,
    0.0996071f,
    0.088714f,
    0.0790123f,
    0.0703715f};

const float Observer::A_values[9] =
{
    1.00000f,    0.02000f,    0.00000f,
    0.75385f,    1.00000f,   -0.75385f,
    13.26021f,    2.15985f,   -1.44739f
};

const float Observer::b_values[3] =
{ 0.0f,
  0.0f,
  0.02f };

const float Observer::L_values[3] =
{
    0.074599f,
    -0.631355f,
  -0.330745f
};

const float Observer::c_values[3] =
{ 0.0f, 0.0f, 1.0f };

const float Observer::Gi = -28.705f;

Observer::Observer()
    : WalkController(), stateVector(ufvector3(3)),
      A(ufmatrix3(3,3)), b(ufvector3(3)), c(ufrowVector3(1,3)),
      L(ufvector3(3)),trackingError(0.0f)
      {
    // instantiate the ublas matrices with their respective values
    // TODO: there might be a better way to do this.
    for (int i=0; i < 3; i++)
        stateVector(i) = 0.0f;

    for (int i=0; i < 3; i++)
        A(0, i) = A_values[i];
    for (int i=0; i < 3; i++)
        A(1, i) = A_values[3+i];
    for (int i=0; i < 3; i++)
        A(2, i) = A_values[6+i];

    for (int i=0; i < 3; i++)
        b(i) = b_values[i];

    for (int i=0; i < 3; i++)
        c(0,i) = c_values[i];

    for (int i=0; i < 3; i++)
        L(i) = L_values[i];

#ifdef DEBUG_CONTROLLER_GAINS
    FILE * gains_log;
    gains_log = fopen("/tmp/gains_log.xls","w");
    int j = 0;
    fprintf(gains_log,"time\tgain\n");
    //write the controller gains
    for(unsigned int i  = 0; i < NUM_PREVIEW_FRAMES; i++){
        fprintf(gains_log,"%d\t%f\n",j,weights[j]);
        j++;
    }
    fclose(gains_log);
#endif

}

/**
 * Tick calculates the next state vector for the robot, given the zmp_ref
 *
 */
const float Observer::tick(const list<float> *zmp_ref,
                           const float cur_zmp_ref,
                           const float sensor_zmp) {
    float preview_control = 0.0f;
    unsigned int counter = 0;

    for (list<float>::const_iterator i = zmp_ref->begin();
         counter < NUM_PREVIEW_FRAMES; ++counter, ++i) {
        preview_control += weights[counter]* (*i);
    }

    trackingError += prod(c,stateVector)(0) - cur_zmp_ref;

    const float control = -Gi * trackingError - preview_control;
    const float psensor = sensor_zmp;

    ufvector3 temp(prod(A, stateVector)
                   - L*(psensor - prod(c,stateVector)(0)) * 1.0f
                   + b*control);
    stateVector.assign(temp);

    return getPosition();
}

/**
 * Initialize the position of the robot (vel and accel assumed to be 0)
 * We also assume we are starting off without any tracking error.
 */
void Observer::initState(float x, float v, float p){
    stateVector(0) = x;
    stateVector(1) = v;
    stateVector(2) = p;
    trackingError = 0.0f;
}
