asgn1 练习MVP变换。<br/>
model transformation：对空间中物体的变换，通过旋转平移缩放等，摆放好物体。
view transformation：对相机的变换，将空间中任意相机平移到(0,0,0)点，并且看向-z方向。
                    view transformation也要对物体使用，这样才能保证相机看到的东西不变。
project transformation：将物体投影到相机平面上。
        正交投影(orthographic projection)：
            字面上理解正交投影只要舍弃Z轴就行，但是在实际应用中：
            正交投影将一个长方体空间变换到一个[-1,1]^3的标准正方体，保留z值用作记录深度信息。
        透视投影(perspective projection)
            透视投影的空间是一个视锥(frustum)。
            透视投影的实现就是将这个视锥挤压成一个长方体，然后对这个长方体进行正交投影。