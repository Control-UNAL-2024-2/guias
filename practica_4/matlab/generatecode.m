function generatecode(Ab,Bb, C, D, L)
% this function generates code for a difital controller
code = "float computeController(float e, float umin, float umax){\n";
code = code + "    // This function computes the control law for a discretized controller Cd(z)\n";
code = code + "    // e = ref - y is the current error in the controlled system\n";
code = code + "    // and umin and umax are the limits of the control signal\n\n"; 
code = code + "    // using the following state space representation\n";
code = code + "    //       x[n+1]= Ab*x[n] + Bb*e[n]\n";
code = code + "    //       u[n]= sat(C*x[n] + D*e[n])\n";
code = code + "    // where  Ab = A - L*C and Bb = B - L*D\n";
code = code + "    // with  L the gain of a Kalman Filter";
code = code + "\n\n";

code = code + "    // The following constants define the Ab matrix\n";
for i = 1:size(Ab,1)  % Filas
    for j = 1:size(Ab,2)  % Columnas
        code = code + sprintf("    const float aw%d%d = %.20f;\n", i, j, single(Ab(i,j)));
    end
        
end
code = code + "\n";
code = code + "    // The following constants define the Bb matrix\n";
for i = 1:size(Bb,1)  % Filas
        code = code + sprintf("    const float bw%d = %.20f;\n", i, single(Bb(i)));  
end
code = code + "\n";
code = code + "    // The following constants define the C matrix\n";
for i = 1:size(C,2)  % Filas
        code = code + sprintf("    const float c%d = %.20f;\n", i, single(C(i)));  
end
code = code + "\n";
code = code + "    // The following constant define the D scalar\n";
for i = 1:size(D,2)  % Filas
        code = code + sprintf('    const float d%d = %.20f;\n', i, single(D(i)));  
end
code = code + "\n";
code = code + "    // The following constants define the antwindup vector L\n";
for i = 1:size(L,1)  % Filas
        code = code + sprintf('    const float l%d = %.20f;\n', i, single(L(i)));  
end

code = code + "\n";
code = code + "    // The following variables represent the states x[n]\n";
code = code + "    // in the state-space representation. They must be declared\n";
code = code + "    // as static to retain their values between function calls.\n";
for i = 1:size(Bb,1)  % Filas
    code = code + sprintf("    static float x%d = 0;\n", i);  
end
code = code + "\n";
code = code + "    // The following variables are the new computed states x[n+1]\n";
code = code + "    // of the state space representation\n";
for i = 1:size(Bb,1)  % Filas
       code = code + sprintf('    float x%d_new = 0;\n', i);  
end
code = code + "\n";
code = code + "    // The following variable is the control signal. \n";
code = code + "    // it also must be declared as static to retain its value between function calls. \n";
code = code + sprintf("    static float u = 0;\n");
code = code + "\n";

code = code + "    /*************************************************\n";
code = code + "                THIS IS THE CONTROLLER'S CODE\n";
code = code + "    **************************************************/\n\n";
code = code + sprintf("    // Compute the new predicted state x[n+1] = Ab*x[n] + Bb*e[n] + L*u[n]\n");

for i = 1:size(Ab,1)  % Filas
    code = code + sprintf('    x%d_new = ', i);
    for j = 1:size(Ab,2)  % Columnas
        code = code + sprintf('aw%d%d*x%d + ', i, j, j);    
    end
    code = code + sprintf('bw%d*e + l%d*u;', i,i);
    code = code + "\n";    
end
code = code + "\n";
code = code + sprintf("    // Compute the control output u[n] = C*x[n] + D*e[n]\n");
code = code + sprintf('    u = ');
for i = 1:size(C,2)  % Filas
    code = code + sprintf('c%d*x%d + ', i, i);   
end
for i = 1:size(D,2)  % Filas
        code = code + sprintf('d%d*e;', i); 
end
code = code + "\n\n"; 
code = code + "    // Saturate control signal\n";
code = code + "    u = constrain(u, umin, umax);\n";
code = code + "\n"; 

code = code + "    // Make the predicted state the current state: x[n] <- x[n+1]\n";
for i = 1:size(Ab,1)
    code = code + sprintf("    x%d = x%d_new;\n", i, i);
end
code = code + "\n";
code = code + "    // now, the control signal is available to the main control routine\n";
code = code + "    return u;\n";
code = code + "}\n";

fh = fopen('controller.h', 'w');
fprintf(fh,  code);
fclose(fh);
disp("code for controller has been generated")
return
