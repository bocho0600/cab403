#include <stdio.h>
#define PI 3.14
float CylinderVolume(float radius, float height);
float radius, height, Volume;
void main(){
      printf("Enter the radius of the cylinder: ");
      scanf("%f", &radius);
      printf("Enter the height of the cylinder: ");
      scanf("%f", &height);
      Volume = CylinderVolume(radius, height);
      printf("The volume of the cylinder is: %.2f\n", Volume);
}

float CylinderVolume(float radius, float height){
      return PI * radius * radius * height;
}