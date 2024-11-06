import numpy as np
import pybullet as p
import pybullet_data
import time
import gymnasium as gym

p.connect(p.GUI)
p.resetSimulation()
p.setAdditionalSearchPath(pybullet_data.getDataPath())

p.setGravity(0,0,-9.8)
p.setRealTimeSimulation(0)



p.loadURDF("plane.urdf", [0,0,0], [0,0,0,1])
pendulum = p.loadURDF("./Proyecto/InvertedPendulum/twowheel.urdf", [0,0,0.15], [0,0,0,1])

obj_of_focus = pendulum


joint_left = p.getJointInfo(pendulum, 0)
joint_right = p.getJointInfo(pendulum, 1)


jlower = joint_left[8]
jupper = joint_left[9]

print(jlower, jupper)

for step in range(300):
    joint_two_targ = np.random.uniform(jlower, jupper)
    joint_four_targ = np.random.uniform(jlower, jupper)
    
    p.setJointMotorControlArray(pendulum, [0,1], p.POSITION_CONTROL, [2, -2])

    print(p.getLinkState(pendulum, 1, computeForwardKinematics=True, computeLinkVelocity=True)[6])

    focus_position, _ = p.getBasePositionAndOrientation(pendulum)
    
    p.resetDebugVisualizerCamera(cameraDistance=3, cameraYaw=0, cameraPitch=-40, cameraTargetPosition = focus_position)
    p.stepSimulation()
    time.sleep(.01)
