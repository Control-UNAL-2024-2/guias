float computeController(float e, float umin, float umax){
    // This function computes the control law for a discretized controller Cd(z)
    // e = ref - y is the current error in the controlled system
    // and umin and umax are the limits of the control signal

    // using the following state space representation
    //       x[n+1]= Ab*x[n] + Bb*e[n]
    //       u[n]= sat(C*x[n] + D*e[n])
    // where  Ab = A - L*C and Bb = B - L*D
    // with  L the gain of a Kalman Filter

    // The following constants define the Ab matrix
    const float aw11 = 0.92190784215927124023;
    const float aw12 = 0.00414303503930568695;
    const float aw21 = 1.74990832805633544922;
    const float aw22 = 0.81766259670257568359;

    // The following constants define the Bb matrix
    const float bw1 = 0.00012232111475896090;
    const float bw2 = 0.00084393681026995182;

    // The following constants define the C matrix
    const float c1 = -0.06108257919549942017;
    const float c2 = -0.00654000043869018555;

    // The following constant define the D scalar
    const float d1 = 0.05182243138551712036;

    // The following constants define the antwindup vector L
    const float l1 = 1.86796653270721435547;
    const float l2 = 1.50687634944915771484;

    // The following variables represent the states x[n]
    // in the state-space representation. They must be declared
    // as static to retain their values between function calls.
    static float x1 = 0;
    static float x2 = 0;

    // The following variables are the new computed states x[n+1]
    // of the state space representation
    float x1_new = 0;
    float x2_new = 0;

    // The following variable is the control signal. 
    // it also must be declared as static to retain its value between function calls. 
    static float u = 0;

    /*************************************************
                THIS IS THE CONTROLLER'S CODE
    **************************************************/

    // Compute the new predicted state x[n+1] = Ab*x[n] + Bb*e[n] + L*u[n]
    x1_new = aw11*x1 + aw12*x2 + bw1*e + l1*u;
    x2_new = aw21*x1 + aw22*x2 + bw2*e + l2*u;

    // Compute the control output u[n] = C*x[n] + D*e[n]
    u = c1*x1 + c2*x2 + d1*e;

    // Saturate control signal
    u = constrain(u, umin, umax);

    // Make the predicted state the current state: x[n] <- x[n+1]
    x1 = x1_new;
    x2 = x2_new;

    // now, the control signal is available to the main control routine
    return u;
}
