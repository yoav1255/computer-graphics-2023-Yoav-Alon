## Paragraph 1:
We implemented an Orthographic projection that enables the user to control the view volume :

<img src='./1a.png'>
<img src='./1b.png'>

Video for demonstration :


https://user-images.githubusercontent.com/101698622/208249500-6205efa4-c490-4b42-a488-8a7b03e60807.mp4


## Paragraph 2:

Reposition the camera in camera frame and in world frame :

<img src='./2a.png'>

video for demostration :



https://user-images.githubusercontent.com/101698622/208249833-c38ab71f-3ae5-444a-93d7-601f88741c8b.mp4

In order to reposition the camera in the camera frame and in the world frame we multiplied the following:
camera projection X inverse(camera local frame X camera world frame) X matrix transformation

## Paragraph 3:

allowing the user to resize the window's size and we will resize the viewport accordingly with a new aspect ratio:

<img src='./3a.png'>
<img src='./3b.png'>
<img src='./3c.png'>


## Paragraph 4:

Axes drawing :

Translating and rotating on world frame: 
<img src='./4a.png'>
Translating in local frame and rotating in world frame:
<img src='./4b.png'>
Translating and rotating on local frame: 
<img src='./4c.png'>

## Paragraph 5:
Applying T1 as a translate matrix with x = 0.889 and T2 as a rotation matrix with 209 degrees rotation relative to the Z axis.

T1 on model frame then T2 on world frame:
![Screenshot (16)](https://user-images.githubusercontent.com/108798956/208262472-f660eafc-83bb-492c-bdac-b3cc6ab86ee1.png)

T1 on world frame then T2 on model frame:
![Screenshot (17)](https://user-images.githubusercontent.com/108798956/208262488-81c9f035-6294-4458-8b94-abe2c5b0d713.png)

We can see that the diffrence is that the order of multiplycation as well as the matrices applied to each of the frames makes a diffrence as T1,T2 are non commuting transformation.


## Paragraph 6:

Bounding Box Drawing :
The bounding box in the local frame is transforming together with the model,
while the bounding box in the world frame is always parallel to the World Axis.
<img src='./6aNew.png'>

## Paragraph 7:

We can see that translating and rotating doesn't affect the vertices.
with a cube it is clear to see that the normals are perpendicular to the faces.
Vertex Normals
<img src='./7a.png'>
<img src='./7b.png'>

Face Normals

<img src='./7c.png'>
<img src='./7d.png'>

## Paragraph 8:

Orthographic projection compared to perspective projection.
As a sanity check we moved on the camera's Z axis, and it is clear to see that on perspective projection the object looks farther campared to the orthographic projection.
Orthographic:

<img src='./8a.png'>
Perspective:

<img src='./8b.png'>


## Paragraph 9 video:


https://user-images.githubusercontent.com/108798956/208097507-5016801e-be26-46bf-a712-faf5f0cd8fc2.mp4

## Paragraph 10 screenshot:


![Screenshot (10)](https://user-images.githubusercontent.com/108798956/208098804-1105a309-3426-4ea2-b250-67a48d914290.png)

## Paragraph 11 video - Dolly zoom:
### Manipulating the camera's FOV with it's movement on the Z asix causes  this "vertigo" effect.


https://user-images.githubusercontent.com/108798956/208257643-d1310f0e-23d8-4b6f-8c66-ad421ca21898.mp4


## Paragraph 12 - video of adding another camera to an existing one and 2 screenshots:


https://user-images.githubusercontent.com/108798956/208258205-0bab2172-133b-4102-9e6f-9413bb209ded.mp4

![Screenshot (11)](https://user-images.githubusercontent.com/108798956/208258290-b68c6eff-240f-45b8-b3c6-6c36d866bc50.png)

![Screenshot (12)](https://user-images.githubusercontent.com/108798956/208258310-231d2b86-f4e6-4387-900a-fb8d995f0b81.png)

## Paragraph 13:


![Screenshot (18)](https://user-images.githubusercontent.com/108798956/208263059-e6c5ced0-d804-4749-aa80-dd1e2a8a5673.png)



